#!/usr/bin/env python

from __future__ import print_function
import glob, os, re
from subprocess import check_output

# Run benchmark for one method.
def run_benchmark(method, files):
  print('Benchmarking {}...'.format(method))
  times = []
  for line in check_output(['./nl-benchmark', method] + files).splitlines():
    m = re.match(r'(.*)\t(.*)', line)
    filename, time = m.group(1), float(m.group(2))
    index = len(times)
    if filename != files[index]:
      raise Exception('filenames don\'t match: {} {}'.format(filename, files[index]))
    times.append(time)
  return times

repeat_times = 3

methods = ['mp-read', 'mp-build', 'asl']
method_names = {'io': 'I/O', 'mp-read': 'nl reader', 'mp-build': 'nl reader+build', 'asl': 'ASL'}
for format in ['text', 'binary']:
  files = glob.glob(os.path.join(os.path.dirname(__file__), 'cute/' + format + '/*.nl'))
  times = {}
  for i in range(repeat_times):
    for method in methods:
      time = 0
      for t in run_benchmark(method, files):
        time += t
      times[method] = min(times.get(method, float('inf')), time)
      print(time)
  with open('nl-' + format + '.csv', 'w') as f:
    for m in methods:
      f.write('{},{:.2f}\n'.format(method_names[m], times[m]))
