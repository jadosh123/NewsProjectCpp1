// ConnectedApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include "CommonObject.h"
#include "DrawThread.h"
#include "DownloadThread.h"
#include "Favorite.h"


int main()
{ 
    CommonObjects common;
    //To load the user's favorites before program launch
    FavoriteNews fav;
    auto fav_th = std::jthread([&] {fav(common); });//TESTING TESTING
    fav_th.join();

    //Threads to draw graphics and to download the homepage content
    DrawThread draw;
    auto draw_th = std::jthread([&] {draw(common); });
    DownloadThread down;
    auto down_th = std::jthread([&] {down(common); });
    down.SetUrl("http://....");
    std::cout << "running...\n";

    //fav_th.join();
    draw_th.join();
    down_th.join();
}

