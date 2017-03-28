#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <memory>
#include <thread>
#include <iostream>
#include <fstream>

#include "sony_remote_camera.h"

using namespace std;
using namespace src;

int main(int, char**) {
  Sony_Remote_Camera_Interface *s;
  s = GetSonyRemoteCamera("192.168.122.153");
  if(s == nullptr) {
    cout << "ERROR\n";
    return 1;
  }
  s->Launch_Liveview();
  delete s;
/*  cout << "2\n";
  uint8_t *img;
  size_t size;
  int ts, fn;
  cout << "3\n";
  auto err = s->Get_Last_JPeg_Image(img, size, fn, ts);
  cout << "HERE\n";
  while(true) {
    err = s->Get_Last_JPeg_Image(img, size, fn, ts);
    this_thread::sleep_for(chrono::milliseconds(200));
  }
  if(err == SC_NO_NEW_DATA_AVAILABLE) cout << "HI\n";
  cout << "4\n";
  this_thread::sleep_for(chrono::milliseconds(2000));
  //err = s->Get_Last_JPeg_Image(img, size, fn, ts);
  cout << "5\n";
  if(err != SC_NO_ERROR) cout << "ERR\n";
  vector<uint8_t> start_marker = {0xff, 0xd8};
  vector<uint8_t> end_marker = {0xff, 0xd9};
  auto start =
      search(img, img + size, start_marker.begin(), start_marker.end());
  auto end = search(img, img + size, end_marker.begin(), end_marker.end());
  if(start == img + size) cout << "BAD\n";
  if(end == img + size) cout << "BAD\n";
  this_thread::sleep_for(chrono::milliseconds(200));
  free(img);
  err = s->Get_Last_JPeg_Image(img, size, fn, ts);
  if(err != SC_NO_ERROR) cout << "ERR\n";
  start = search(img, img + size, start_marker.begin(), start_marker.end());
  end = search(img, img + size, end_marker.begin(), end_marker.end());
  if(start == img + size) cout << "BAD\n";
  if(end == img + size) cout << "BAD\n";
  free(img);*/
}
