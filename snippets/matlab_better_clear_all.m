% A better "clear all" without removing the breakpoints
s=dbstatus;
tmp_folder = tempdir;
save([tmp_folder, 'myBreakpoints.mat'], 's');
clear all
close all
clear functions
clear globals
tmp_folder = tempdir;
load([tmp_folder, 'myBreakpoints.mat']);
dbstop(s);
