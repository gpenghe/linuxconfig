#
# 2012-10, gph
# 

import datetime
import threading        
class ThreadClass(threading.Thread):
          def run(self):
            now = datetime.datetime.now()
            print "%s says Hello World at time: %s" % (self.getName(), now)
        
for i in range(2):
          t = ThreadClass()
          t.start()
