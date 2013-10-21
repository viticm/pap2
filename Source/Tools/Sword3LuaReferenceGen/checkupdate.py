from subprocess import Popen, PIPE
import os
import os.path
import sys
import re
from checklist import *



os.environ['CVSROOT'] = CVS_ROOT

def check(f):
    exe = '"%s" "status" "%s"' % (CVS_EXE, os.path.basename(f))
    p = Popen(exe,
        stdout=PIPE, stderr=PIPE,
        cwd=os.path.dirname(f))
    out, err = p.communicate()
    if err.strip() != '':
        return False, err
    return True, out
    
def need_update(out):
    reWorking = re.compile(r'Working\s+revision:\s+(\d*\.\d*)')
    reRepository = re.compile(r'Repository\s+revision:\s+(\d*\.\d*)')
    m = reWorking.search(out)
    working, repository = None, None
    
    if m:
        working = m.group(1)
    m = reRepository.search(out)
    if m:
        repository = m.group(1)
    if working is None or repository is None:
        sys.stderr.write('Unknown Error!\n')
    if working != repository:
        return True
    return False
    
for root, files in CHECK_FILES:
    for f in files:
        result, msg = check(os.path.join(root, f))
        if result:
            if need_update(msg):
                print 'Update:', f
        else:
            sys.stderr.write('Error: %s' % msg)
            
        