// Copyright (c) 2011, Yuya Unno.
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of Yuya Unno nor the names of other
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <pficommon/network/cgi.h>
#include <pficommon/network/cgi/xhtml_cgi.h>
#include <pficommon/network/cgi/server.h>

#include <mecab.h>

using namespace std;
using namespace pfi::network::cgi;

class my_cgi : public xhtml_cgi, cloner<my_cgi> {
 public:

  my_cgi() {
    tagger_ = MeCab::createTagger("");
    if (!tagger_) {
      string msg = MeCab::getTaggerError();
      throw runtime_error(msg);
    }
  }

  void run() {
    string path = env["SCRIPT_NAME"];
    if (path == "/parse") { parse(); }
    else { top(); }
  }

  void form(const string& s) {
    h1__ { text__("mecab sample"); }
    form__ { action__ = "parse";
      input__ { type__ = "text"; name__ = "s"; size__ = "100"; value__ = s;}
      input__ { type__ = "submit"; }
    }
  }

  void top() {
    html__ {
      body__ {
        form("");
      }
    }
  }

  void parse() {
    string s = query["s"];
    const MeCab::Node* node = tagger_->parseToNode(s.c_str());
    html__ {
      body__ {
        form(s);
        table__ { border__ = "1";
          tr__ {
            th__ { text__("surface"); }
            th__ { text__("feature"); }
          }
          for (; node; node = node->next) {
            tr__ {
              td__ {
                string surface(node->surface, node->length);
                text__(surface);
              }
              td__ {
                text__(node->feature);
              }
            }
          }
        }
      }
    }
  }

 private:
  MeCab::Tagger* tagger_;
};

int main(int argc, char* argv[]) {
  run_server(my_cgi(), argc, argv).run(true);
}
