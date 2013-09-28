#!/usr/bin/env python

import sys, re, subprocess

def usage():
    print '''\
Usage: %s <program name> <test file>
''' % sys.argv[0]

def escape(s, code):
    return '\033[%sm%s\033[0m' % (code, s)

def red(s):
    return escape(s, 91)

def green(s):
    return escape(s, 92)

def main():
    try:
        prog_name, fin_name = sys.argv[1:]
    except ValueError:
        usage()
        sys.exit(1)

    pat = re.compile('^(.+) (.+)$')
    out_pat = re.compile('^(.*)\n$')

    failures = 0
    passes = 0

    def test_pass(s):
        print green('PASS') + ': ' + s

    def test_fail(s):
        print red('FAIL') + ': ' + s

    with open(fin_name, 'r') as fin:
        for line in filter(None, fin):
            m = pat.match(line)
            if m:
                text_in, expected_out = m.groups()
                p = subprocess.Popen([prog_name, text_in], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                rc = p.wait()
                raw_out = p.stdout.read()
                m = out_pat.match(raw_out)
                actual_out = m and m.group(1)
                if rc != 0:
                    test_fail('%s => %s: [%d] %s' % (text_in, expected_out, rc, raw_out.rstrip()))
                    failures += 1
                elif expected_out == actual_out:
                    test_pass('%s: %s == %s' % (text_in, expected_out, actual_out))
                    passes += 1
                else:
                    test_fail('%s: %s != %s' % (text_in, expected_out, actual_out))
                    failures += 1
        print '%d/%d passed' % (passes, passes + failures)
        if failures:
            print red('FAILURE')
        else:
            print green('SUCCESS')

if __name__ == '__main__':
    try:
        main()
    except IOError, e:
        # Ignore broken pipe (e.g. piping into head)
        if e.errno != 32:
            raise e

