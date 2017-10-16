reset
set table 'load_time_cpy.dat'
plot [0:100] 'bench_load_cpy.txt' using 1
unset table

plot 'load_time_cpy.dat' u 1:2

f(x) = cpy_mean_y
fit f(x) 'load_time_cpy.dat' u 1:2 via cpy_mean_y
cpy_stddev_y = sqrt(FIT_WSSR / (FIT_NDF + 1 ))

set table 'load_time_ref.dat'
plot [0:100] 'bench_load_ref.txt' using 1
unset table

plot 'load_time_ref.dat' u 1:2

g(x) = ref_mean_y
fit g(x) 'load_time_ref.dat' u 1:2 via ref_mean_y
ref_stddev_y = sqrt(FIT_WSSR / (FIT_NDF + 1 ))

set ylabel 'time(sec)'
set style fill solid
set title 'performance comparison (loading words)'
set term png enhanced font 'Verdana,10'
set output 'load_time.png'

set label 1 gprintf("CPY-Mean = %g", cpy_mean_y) at 2,0.175
set label 2 gprintf("CPY-Std Dev = %g", cpy_stddev_y) at 2,0.17
set label 3 gprintf("REF-Mean = %g", ref_mean_y) at 2,0.16
set label 4 gprintf("REF-Std Dev = %g", ref_stddev_y) at 2,0.155

plot [0:100] 'bench_load_cpy.txt' using 1 with points title 'CPY', \
'bench_load_ref.txt' using 1 with points title 'REF'
