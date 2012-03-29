/*
Copyright (c) 2012 Mateusz Bieniek

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include <QApplication>
#include <QtGui>
#include <QSharedMemory>

#include "shortcut.h"
/*
  Two arguments means that the program is run in stealth mode.
  In stealth mode the second parameter is the path to the
  file to which the shortcut is going to be created. In stealth mode the
  user interface should not be run.
  */
int main(int argc, char *argv[])
{
    // This doesn't allow creating two instances of this software.
    QSharedMemory mem("shortcutAppMem");
    bool appRunning = false;
    if (mem.attach())
        appRunning = true;
    else
        mem.create(1);


    // do not start the application if one is running
    if (argc == 1 && appRunning == true)
        return 0;

    QApplication a(argc, argv);
    Shortcut mainWindow(argc, argv);

    /* create and run the user interface only if the software was run directly,
      don't run it if the file context menu was used to add a shortcut.
        */
    if (argc == 1){
        int ret = a.exec();
        mem.detach();
        return ret;
    }

    return 0;
}
