/*
 *    Copyright 2023 The ChampSim Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util/units.h"

namespace champsim::data::data_literals
{
    bytes     operator""_B  (unsigned long long val) { return bytes{val}; }
    kibibytes operator""_kiB(unsigned long long val) { return kibibytes{val}; }
    mebibytes operator""_MiB(unsigned long long val) { return mebibytes{val}; }
    gibibytes operator""_GiB(unsigned long long val) { return gibibytes{val}; }
    tebibytes operator""_TiB(unsigned long long val) { return tebibytes{val}; }
}
