#! /bin/bash

exec >code.sgml

files="../invar.h ../invar.c ../apple_serial.c ../apple_pseudo*.c ../apple_mpi.?.c ../apple_mpi_poll*.c ../apple_mpi_nblock*.c ../apple_mpi_dynamic*.c ../apple_omp.1.c ../apple_omp.2.c"

for f in $files; do

    title=`basename $f | sed 's/_/\&#95;/g' `
    id=`basename $f | sed 's/_/./g' `
    
    echo "<appendix id=\"$id\">"
    echo "<title>$title</title>"
    echo "<programlisting>"
    echo " <![CDATA["
    cat $f
    echo "]]>"
    echo "</programlisting>"
    echo "</appendix>"

done

