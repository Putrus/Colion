#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include <windows.h>

void clearScreen()
{
   COORD cursorPosition;
   cursorPosition.X = 0;
   cursorPosition.Y = 0;
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

std::vector<std::vector<std::wstring>> loadSprite(const char* filename)
{
   std::vector<std::vector<std::wstring>> sprite;
   std::fstream file(filename);
   if (file.bad() || file.fail() || !file.is_open())
   {
      std::cout << "Failed to open file" << std::endl;
      return {};
   }
   int frames;
   int height;
   file >> frames >> height;
   std::string line;
   std::getline(file, line);

   for (int frame_nr = 0; frame_nr < frames; ++frame_nr)
   {
      std::vector<std::wstring> frame;
      for (int y = 0; y < height; ++y)
      {
         std::getline(file, line);
         int length = MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, NULL, 0);
         if (length == 0)
         {
            std::cerr << "Error converting string to UTF-16" << std::endl;
            return {};
         }
         std::vector<wchar_t> buffer(length);
         MultiByteToWideChar(CP_UTF8, 0, line.c_str(), -1, buffer.data(), buffer.size());
         frame.push_back(std::wstring(buffer.data(), length - 1));
      }
      std::getline(file, line);
      sprite.push_back(frame);
   }
   file.close();
   return sprite;
}

void updateSprite(int frames, int& frame_nr, int& dist, int dist_incr, const std::vector<std::vector<std::wstring>>& sprite)
{
   for (const auto& row : sprite[frame_nr])
   {
      
      int length = WideCharToMultiByte(CP_UTF8, 0, row.c_str(), -1, NULL, 0, NULL, NULL);
      if (length == 0)
      {
         std::cerr << "Error converting string to UTF-8" << std::endl;
         return;
      }
      std::vector<char> buffer(length);
      WideCharToMultiByte(CP_UTF8, 0, row.c_str(), -1, buffer.data(), buffer.size(), NULL, NULL);
      
      for (int i = 0; i < dist; ++i)
      {
         std::cout << ' ';
      }
      std::cout << buffer.data();
      for (int i = dist; i < 118 - dist; ++i)
      {
         std::cout << ' ';
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
   frame_nr = (frame_nr + 1) % frames;
   dist = dist + dist_incr;
}

int main()
{
   SetConsoleOutputCP(CP_UTF8);

   std::vector<std::vector<std::wstring>> sprite_right = loadSprite("res/sprites/run_right.txt");
   std::vector<std::vector<std::wstring>> sprite_left = loadSprite("res/sprites/run_left.txt");
   
   auto start = std::chrono::steady_clock::now();
   float t = 0.f;
   int frame_nr = 0;
   int dist = 5;
   bool right = true;
   int frames = 8;
   while (true)
   {
      auto end = std::chrono::steady_clock::now();
      t += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
      start = end;

      while (t > 50000000)
      {
         clearScreen();
         t -= 50000000;
         if (right)
         {
            updateSprite(frames, frame_nr, dist, 1, sprite_right);
         }
         else
         {
            updateSprite(frames, frame_nr, dist, -1, sprite_left);
         }
         if (dist < 5)
         {
            right = true;
         }
         else if(dist > 50)
         { 
            right = false;
         }
      }
   }
   std::cout << "Hello Colion!" << std::endl;
   return 0;
}