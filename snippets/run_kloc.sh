#!/bin/bash 

# Trace a build of the BB project
#
make clean

rm -rf kwinject.trace trace_out_debug.log

# kwinject --debug -T kwinject.trace make >trace_out_debug.log 2>&1
kwinject --debug -T kwinject.trace make

rm -rf kwinject.out trace_in_debug.log

# kwinject --debug -t kwinject.trace -o kwinject.out >trace_in_debug.log 2>&1
kwinject --debug -t kwinject.trace -o kwinject.out


# Verify the results
#
echo "***** Number of compilations"
grep ^compile kwinject.out | wc -l

echo "***** Compilers configured:"
grep ^config kwinject.out | awk -F ';' '{print $2}'

echo "***** Compilers possibly used:"
grep '"executable":' kwinject.trace | awk -F',' '{print $4}' | awk -F':' '{print $2}' | sort -u


# Run the analysis
#
rm -rf kwbuildproject_debug.log

# kwbuildproject --url http://xxx.com:8090/BB --license-host xxx.com --license-port 27010 -debug -f -o kwtables kwinject.out  2>&1 | tee kwbuildproject_debug.log
kwbuildproject --url http://xxx.com:8090/BB --license-host xxx.com --license-port 27010 -debug -f -o kwtables kwinject.out


# Load the results into the Portal
#
rm -rf kwadmin_load_debug.log

# kwadmin --url http://xxx.com:8090 --debug load BB kwtables >kwadmin_load_debug.log 2>&1
kwadmin --url http://xxx.com:8090 --debug load BB kwtables

# tar -czf kwdiagnostics_bb.tgz kwinject.trace kwinject.out trace_out_debug.log trace_in_debug.log kwbuildproject_debug.log kwadmin_load_debug.log kwtables/build.log

