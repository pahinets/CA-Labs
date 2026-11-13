#!/usr/bin/env python3
"""Real kernel integration checks; run as root after make."""
import os
from pathlib import Path
import re
import subprocess
import unittest

BASE = Path(__file__).resolve().parent
RESULTS = BASE / 'results'
RESULTS.mkdir(exist_ok=True)


def command(*args, expected=0):
    result = subprocess.run(args, cwd=BASE, text=True,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    print('$ ' + ' '.join(args), flush=True)
    print(result.stdout, end='' if result.stdout.endswith('\n') else '\n', flush=True)
    print(f'[exit {result.returncode}]', flush=True)
    if expected is not None:
        assert result.returncode == expected, result.stdout
    return result


def kernel_log():
    return subprocess.check_output(['dmesg', '--raw'], text=True).splitlines()


class ModuleTests(unittest.TestCase):
    def setUp(self):
        self.assertEqual(os.geteuid(), 0, 'Run as root')
        self.assertFalse(Path('/sys/module/hello1').exists(),
                         'hello1 already loaded; will not touch it')
        self.assertFalse(Path('/sys/module/hello2').exists(),
                         'hello2 already loaded; will not touch it')

    def tearDown(self):
        for module in ('hello2', 'hello1'):
            if Path('/sys/module', module).exists():
                command('rmmod', module)

    def test_default(self):
        self.assertTrue((BASE / 'hello1.ko').is_file(), 'Build hello1.ko first')
        self.assertTrue((BASE / 'hello2.ko').is_file(), 'Build hello2.ko first')
        before = len(kernel_log())
        command('insmod', './hello1.ko')
        command('insmod', './hello2.ko')
        self.assertEqual(command('cat', '/sys/module/hello2/parameters/howmany').stdout.strip(), '1')
        command('rmmod', 'hello2')
        command('rmmod', 'hello1')
        log = '\n'.join(kernel_log()[before:])
        print(log, flush=True)
        self.assertEqual(log.count('hello1: Hello, world!'), 1)
        durations = re.findall(r'hello1: print \d+ took (\d+) ns', log)
        self.assertEqual(len(durations), 1)
        self.assertTrue(all(int(value) >= 0 for value in durations))
        (RESULTS / 'default.dmesg').write_text(log + '\n')

    def test_parameter_cases(self):
        for value in [*range(12), 15]:
            with self.subTest(howmany=value):
                before = len(kernel_log())
                command('insmod', './hello1.ko')
                result = command('insmod', './hello2.ko', f'howmany={value}', expected=None)
                if value > 10:
                    self.assertNotEqual(result.returncode, 0)
                    self.assertFalse(Path('/sys/module/hello2').exists())
                else:
                    self.assertEqual(result.returncode, 0)
                    self.assertEqual(command('cat', '/sys/module/hello2/parameters/howmany').stdout.strip(), str(value))
                    command('rmmod', 'hello2')
                command('rmmod', 'hello1')
                log = '\n'.join(kernel_log()[before:])
                print(log, flush=True)
                (RESULTS / f'howmany-{value}.dmesg').write_text(log + '\n')
                count = value if value <= 10 else 0
                self.assertEqual(log.count('hello1: Hello, world!'), count)
                times = re.findall(r'hello1: print \d+ took (\d+) ns', log)
                self.assertEqual(len(times), count)
                if value == 0 or 5 <= value <= 10:
                    self.assertRegex(log, rf'<4>.*hello2: howmany = {value}, continuing anyway')
                elif value > 10:
                    self.assertRegex(log, rf'<3>.*hello2: howmany = {value} is greater than 10')
                else:
                    self.assertNotIn('continuing anyway', log)

    def test_dependency(self):
        before = len(kernel_log())
        result = command('insmod', './hello2.ko', expected=None)
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(Path('/sys/module/hello2').exists())
        log = '\n'.join(kernel_log()[before:])
        self.assertIn('Unknown symbol print_hello', log)
        print(log, flush=True)
        (RESULTS / 'missing-provider.dmesg').write_text(log + '\n')
        command('insmod', './hello1.ko')
        command('insmod', './hello2.ko', 'howmany=3')
        command('lsmod')
        self.assertTrue(Path('/sys/module/hello1/holders/hello2').exists())
        result = command('rmmod', 'hello1', expected=None)
        self.assertNotEqual(result.returncode, 0)
        self.assertTrue(Path('/sys/module/hello1').exists())
        command('rmmod', 'hello2')
        command('rmmod', 'hello1')

    def test_accumulated_events(self):
        before = len(kernel_log())
        command('insmod', './hello1.ko')
        for count in (2, 3):
            command('insmod', './hello2.ko', f'howmany={count}')
            command('rmmod', 'hello2')
        self.assertNotIn('took', '\n'.join(kernel_log()[before:]))
        command('rmmod', 'hello1')
        log = '\n'.join(kernel_log()[before:])
        self.assertEqual(log.count('hello1: Hello, world!'), 5)
        self.assertEqual(len(re.findall(r'hello1: print \d+ took \d+ ns', log)), 5)
        self.assertIn('freed 5 events', log)
        print(log, flush=True)
        (RESULTS / 'accumulated.dmesg').write_text(log + '\n')

    def test_metadata(self):
        command('modinfo', './hello1.ko')
        command('modinfo', './hello2.ko')
        self.assertEqual(command('modinfo', '-F', 'depends', './hello2.ko').stdout.strip(), 'hello1')
        self.assertIn('howmany:', command('modinfo', '-F', 'parm', './hello2.ko').stdout)
        release = command('uname', '-r').stdout.strip()
        for module in ('hello1', 'hello2'):
            self.assertTrue(command('modinfo', '-F', 'vermagic', f'./{module}.ko').stdout.startswith(release + ' '))
        self.assertIn(' U print_hello', command('nm', '-u', './hello2.ko').stdout)
        self.assertRegex(command('nm', './hello1.ko').stdout, r' T print_hello\b')


if __name__ == '__main__':
    unittest.main(verbosity=2)
