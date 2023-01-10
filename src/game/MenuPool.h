#pragma once

struct Menu {
    wchar_t name[64];
    wchar_t* title;
    int data[50];
    int (*render)();
};

namespace game
{
    class MenuPool {
    public:
        Menu menus[256];
        int unk3;
        int unk4;
        int menusCount;

        static MenuPool* GetInstance();
        Menu* GetByName(wchar_t* name);
    };
}