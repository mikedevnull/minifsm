#!/usr/bin/env python3

import argparse
import re
import os.path

include_matcher = re.compile(r'\s*#include [<"](fsm/.*)[">].*')
pragma_once_matcher = re.compile(r'\s*#pragma once.*')

parser = argparse.ArgumentParser()
parser.add_argument('inputfile')
parser.add_argument('include_path')

args = parser.parse_args()

included_files = set()


def process_file(filename, basepath):
    if filename in included_files:
        #print('skip: ' + filename)
        return
    # print(filename)
    included_files.add(filename)
    with open(os.path.join(basepath, filename), 'r') as handle:
        for line in handle.readlines():
            if pragma_once_matcher.match(line):
                continue
            m = include_matcher.match(line)
            if m:
                process_file(m.group(1), basepath)
            else:
                print(line.strip())


infile = os.path.relpath(args.inputfile, args.include_path)

process_file(infile, args.include_path)
