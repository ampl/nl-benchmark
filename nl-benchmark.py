#!/usr/bin/env python

from __future__ import print_function
import glob, os, re
from subprocess import check_output

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

files = glob.glob(os.path.join(os.path.dirname(__file__), 'cute/text/*.nl'))
for method in ['io', 'mp', 'asl']:
  times = run_benchmark(method, files)
  time = 0
  for t in times:
    time += t
  print(time)
