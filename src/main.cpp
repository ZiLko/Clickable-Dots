using namespace geode::prelude;

#include <Geode/modify/BoomScrollLayer.hpp>

class $modify(ProBoomScrollLayer, BoomScrollLayer) {

    struct Fields {
        std::unordered_map<CCSprite*, CCMenuItemSpriteExtra*> m_buttons;
    };

    void onDot(CCObject* sender) {
        int page = static_cast<CCNode*>(sender)->getTag();

        LevelEditorLayer::get() || GameManager::get()->getGameVariable("0168") 
            ? instantMoveToPage(page)
            : moveToPage(page); 
    }

    void updateButtons() {
        if (Mod::get()->getSettingValue<bool>("disable")) return;

        CCArrayExt<CCSprite*> dots = CCArrayExt<CCSprite*>(m_dots);

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

        auto f = m_fields.self();

        for (CCSprite* dot : dots) {
            if (!f->m_buttons.contains(dot)) continue;

            dot->setVisible(false);

            CCMenuItemSpriteExtra* btn = f->m_buttons.at(dot);
            CCSprite* spr = static_cast<CCSprite*>(btn->getNormalImage());

            btn->setPosition(dot->getPosition());
            spr->setScale(dot->getScale());
            spr->setColor(dot->getColor());
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

        int i = 0;

        for (CCSprite* dot : CCArrayExt<CCSprite*>(m_dots)) {
            CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(CCSprite::create("smallDot.png"), this, menu_selector(ProBoomScrollLayer::onDot));
            btn->setTag(i++);

            menu->addChild(btn);

            f->m_buttons[dot] = btn;
        }

        return true;
    }

};