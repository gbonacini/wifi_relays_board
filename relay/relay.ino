// -----------------------------------------------------------------
// Relay Controller - A simple controller for 8 relays board
// Copyright (C) 2020  Gabriele Bonacini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// -----------------------------------------------------------------


#include "relay.h"

using  relay::RelayBoard;

RelayBoard *rb = nullptr;


void setup() {
  
  rb = new RelayBoard("your_ssid", "your_password");
  rb->init();
}

void loop() {
  rb->responder();
}
