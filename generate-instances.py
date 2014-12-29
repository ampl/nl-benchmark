#!/usr/bin/env python
# Generates benchmark instances in the nl format from the AMPL source.

from __future__ import print_function
import glob, os, sys
from subprocess import check_call, PIPE, Popen

DEVNULL = open(os.devnull, 'wb')

def mkdir_if_not_exists(dir):
  if not os.path.exists(dir):
    os.makedirs(dir)

def generate(repo):
  repo_dir = repo + '-repo'
  if not os.path.exists(repo_dir):
    check_call(['git', 'clone', 'https://github.com/ampl/cute.git', repo_dir])
  for dir in ['text', 'binary']:
    mkdir_if_not_exists(os.path.join(repo, dir))
  filenames = glob.glob(repo_dir + '/*.mod')
  index = 0
  for filename in filenames:
    input = ''
    with open(filename) as f:
      for line in f:
        input += line.replace('solve;', '')
    index += 1
    message = '\r[{:3}/{:3}] {}'.format(index, len(filenames), filename)
    sys.stdout.write('{:80}'.format(message))
    out_filename = os.path.splitext(os.path.basename(filename))[0]
    for (fmt, dir) in [('g', 'text'), ('b', 'binary')]:
      path = os.path.join(repo, dir, out_filename)
      Popen(['ampl', '-o' + fmt + path], stdout=DEVNULL, stdin=PIPE).communicate(input)
  sys.stdout.write('\n')

generate('cute')
