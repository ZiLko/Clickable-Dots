using namespace geode::prelude;

#include <Geode/modify/BoomScrollLayer.hpp>

class $modify(ProBoomScrollLayer, BoomScrollLayer) {

    struct Fields {
        std::vector<CCMenuItemSpriteExtra*> m_buttonsArray;
        std::unordered_map<CCSprite*, CCMenuItemSpriteExtra*> m_buttons;

        int m_currentPage = 0;

        bool m_check = false;
        bool m_disable = false;
    };

    void onDot(CCObject* sender) {
        int page = static_cast<CCNode*>(sender)->getTag();

        auto f = m_fields.self();

        if (page == f->m_currentPage) return;

        instantMoveToPage(page + (page > f->m_currentPage ? -1 : 1));

        LevelEditorLayer::get() || GameManager::get()->getGameVariable("0168")
            ? instantMoveToPage(page)
            : moveToPage(page);
    }

    void updateButtons() {
        auto f = m_fields.self();

        if (Mod::get()->getSettingValue<bool>("disable") || f->m_disable) return;
        if (getParent() && !f->m_check) {
            f->m_check = true;

            if (typeinfo_cast<ListButtonBar*>(getParent()))
                f->m_disable = true;
        }

        CCArrayExt<CCSprite*> dots = CCArrayExt<CCSprite*>(m_dots);

        if (static_cast<int>(dots.size()) != static_cast<int>(f->m_buttonsArray.size()))
            addButtons();

        if (dots.size() <= 0) return;

        if (dots[0]->getPosition().x < 0 && Mod::get()->getSettingValue<bool>("fix-position")) {
            float width = CCDirector::get()->getWinSize().width;
            float realWidth = width - 50.f;
            float spacing = realWidth / (dots.size() - 1);
            float screenCenter = width / 2.f;
            float startX = screenCenter - realWidth / 2.f;

            for (int i = 0; i < dots.size(); ++i)
                dots[i]->setPositionX(startX + i * spacing);
        }

        int page = 0;

        for (CCSprite* dot : dots) {
            if (!f->m_buttons.contains(dot)) continue;

            dot->setVisible(false);

            CCMenuItemSpriteExtra* btn = f->m_buttons.at(dot);
            CCSprite* spr = static_cast<CCSprite*>(btn->getNormalImage());

            if (CCNode* parent = getParent())
                if (f->m_disable || parent->getChildByID("dasshu.better-gauntlets/page-navigation")) {
                    btn->setVisible(false);
                    f->m_disable = true;
                    continue;
                }

            btn->setPosition(dot->getPosition());
            spr->setScale(dot->getScale());
            spr->setColor(dot->getColor());

            if (dot->getColor() == ccc3(255, 255, 255))
                f->m_currentPage = page;

            page++;
        }
    }

    void addButtons() {
        auto f = m_fields.self();
        CCNode* menu = getChildByID("buttons-menu"_spr);

        for (CCMenuItemSpriteExtra* btn : f->m_buttonsArray)
            btn->removeFromParentAndCleanup(true);

        f->m_buttonsArray.clear();
        f->m_buttons.clear();

        int i = 0;

        for (CCSprite* dot : CCArrayExt<CCSprite*>(m_dots)) {
            CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(CCSprite::create("smallDot.png"), this, menu_selector(ProBoomScrollLayer::onDot));
            btn->setTag(i++);

            menu->addChild(btn);

            f->m_buttons[dot] = btn;
            f->m_buttonsArray.push_back(btn);
        }
    }

    void updateDots(float dt) {
        BoomScrollLayer::updateDots(dt);

        updateButtons();
    }

    bool init(CCArray* p0, int p1, bool p2, CCArray* p3, DynamicScrollDelegate* p4) {
        if (!BoomScrollLayer::init(p0, p1, p2, p3, p4)) return false;

        if (Mod::get()->getSettingValue<bool>("disable")) return true;

        auto f = m_fields.self();

        CCMenu* menu = CCMenu::create();
        menu->setPosition({0, 0});
        menu->setID("buttons-menu"_spr);

        addChild(menu);

        addButtons();

        // Loader::get()->queueInMainThread([this] {
        //     updateButtons();
        // });

        return true;
    }

};
