#!/usr/bin/env python3
"""Only alphabetic wordsigns and shortforms extend literal Computer8."""
import json
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
from test_computer8 import HOST_KEYS
from test_ueb1 import SOURCES, ROOT

HOST_KEYS = dict(HOST_KEYS, **{"\n": (7 << 16) | 40, "\t": (7 << 16) | 43})
ROWS = json.loads((ROOT/'references/research/ueb-grade2-contractions.yaml').read_text().split('\n',2)[2])['contractions']

class Computer8Short(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp=tempfile.TemporaryDirectory(); cls.addClassCleanup(cls.tmp.cleanup)
        cls.binary=Path(cls.tmp.name)/'short'
        sources=['tests/host/test_computer8_short.c',*SOURCES[1:]]
        subprocess.run(shlex.split(os.environ.get('CC','cc'))+[
            '-std=c11','-Wall','-Wextra','-Werror','-DCONFIG_ZMK_SPLIT=1',
            '-DCONFIG_ZMK_BRAILLE_GRADE2=1','-DCONFIG_ZMK_BRAILLE_COMPUTER8_SHORT=1',
            '-I',str(ROOT/'tests/host/stubs'),'-I',str(ROOT/'include'),
            *[str(ROOT/s) for s in sources],'-o',str(cls.binary)],check=True)
    def observe(self, seq, mode='computer8_short'):
        return json.loads(subprocess.check_output([str(self.binary),mode,*map(str,seq)],text=True))
    def check(self, seq, text):
        observed=self.observe(seq)
        self.assertEqual(observed['keys'],[HOST_KEYS[c] for c in text])
        self.assertEqual(observed['pending'],[])
    def test_all_256_cells_and_commands_preserve_computer8_except_wordsigns(self):
        wordsigns={r['cells'][0]:r['text'] for r in ROWS if r['condition']=='ALPHA'}
        self.assertEqual(len(wordsigns),23)
        for mask in range(256):
            with self.subTest(mask=mask):
                if mask in wordsigns:
                    self.check([mask,256],wordsigns[mask]+' ')
                else:
                    self.assertEqual(self.observe([mask,256])['keys'],
                                     self.observe([mask,256],'computer8')['keys'])
        for mask in (256,257,264,259,265,281,286,273,320,384,328,324,322,321,336):
            with self.subTest(command=mask):
                self.assertEqual(self.observe([mask])['keys'],self.observe([mask],'computer8')['keys'])
    def test_letter_wordsigns_use_standing_alone_context(self):
        wordsigns=[r for r in ROWS if r['condition']=='ALPHA']
        for row in wordsigns:
            mask=row['cells'][0]
            # Every shared-letter wordsign keeps the same UEB2 standing-alone expansion.
            with self.subTest(word=row['text']):
                self.assertEqual(self.observe([mask,256])['keys'],
                                 self.observe([mask,256],'ueb2')['keys'])
                self.assertEqual(self.observe(['preview',mask])['keys'],
                                 [HOST_KEYS[c] for c in row['text']])
                for opening,closing in ((0x37,0x3e),):
                    self.check([opening,mask,closing,256],
                               ('(' if opening==0x37 else '"')+row['text']+
                               (')' if closing==0x3e else '"')+' ')
                # Adjacent letters, digits and slash keep letters literal.
                for seq in ([mask,1],[1,mask,1],[mask,2],[2,mask]):
                    self.assertEqual(self.observe(seq+[256])['keys'],
                                     self.observe(seq+[256],'computer8')['keys'])
                self.assertEqual(self.observe([mask|64,256])['keys'],
                                 self.observe([mask|64,256],'computer8')['keys'])
        for suffix,plain in (([25],"d"),([7,7],"ll"),([23,17],"re"),
                             ([14],"s"),([30],"t"),([39,17],"ve")):
            self.check([61,4]+suffix+[256],"you'"+plain+' ')
        self.check([61,4,14,2,256],"y's1 ")
        self.check([3,12,1,256],'b/a ')
        self.check([1,12,3,256],'a/b ')
        self.check([16,3,16,256],'"but" ')
        # Removed initial-letter contractions now allow quoted wordsigns.
        self.check([16,25,16,256],'"do" ')
        self.check([61,36,3,256],'you-but ')
        self.check([61,40,256],'you. ')
        self.assertEqual(self.observe([3,'reset'])['keys'],[])
        self.assertEqual(self.observe([3,'computer8'])['keys'],[])
        self.check([3,128],'but\n')
        self.check([3,286],'but\t')
        # Equal-length user override remains ahead of a factory wordsign.
        self.check(['override','3','x',3,256],'x ')

    def test_all_75_shortforms(self):
        rows=[r for r in ROWS if r['condition']=='SHORT']
        self.assertEqual(len(rows),75)
        for r in rows:
            with self.subTest(text=r['text']): self.check(r['cells']+[256],r['text']+' ')
    def test_all_45_removed_initial_and_final_contractions_are_literal(self):
        rows=[r for r in ROWS if r['section'] in ('10.7','10.8')]
        self.assertEqual(len(rows),45)
        for r in rows:
            # Enclosing letters avoid the independent standing-alone wordsigns.
            seq=[1]+r['cells']+[1,256]
            with self.subTest(text=r['text']):
                self.assertEqual(self.observe(seq)['keys'],self.observe(seq,'computer8')['keys'])
                self.assertEqual(self.observe([1]+r['cells'][:-1]+['preview',r['cells'][-1]])['keys'],
                                 self.observe([r['cells'][-1]],'computer8')['keys'])
    def test_literal_indicators_uppercase_digits_and_ascii_boundaries(self):
        for seq,text in [([32,32,1],',,a'),([48,48,3],';;b'),([60,1,3],'#ab'),
                         ([0x43,1],'Ba'),([0x41,0x43],'AB'),([2,6,18],'123'),
                         ([1,3,32],'about,'),([32,1,3],',ab'),
                         ([0x37,1,3,0x3e],'(about)'),([16,1,3,16],'"about"'),
                         ([1,3,6],'ab2'),([1,3,0x0c,9],'ab/c')]:
            with self.subTest(seq=seq): self.check(seq+[256],text+' ')
        # Every final groupsign must have a preceding letter; its leading
        # punctuation/digit still has its literal Computer8 meaning otherwise.
        for r in ROWS:
            if r['condition']=='FINAL':
                with self.subTest(final=r['text']):
                    self.assertEqual(self.observe(r['cells']+[256])['keys'],
                                     self.observe(r['cells']+[256],'computer8')['keys'])
    def test_appendix_shortforms_with_literal_computer8_suffixes(self):
        alphabet=[1,3,9,25,17,11,27,19,10,26,5,7,13,29,21,15,31,23,14,30,37,39,58,45,61,53]
        short=sorted((r for r in ROWS if r['condition']=='SHORT'),key=lambda r:-len(r['text']))
        words=json.loads((ROOT/'references/research/ueb-grade2-shortforms.json').read_text())['longer_words']
        for word in words:
            if any(c.isupper() for c in word) or '-' in word: continue
            seq=[];pos=0
            while pos<len(word):
                r=next((r for r in short if word.startswith(r['text'],pos)),None)
                if r: seq+=r['cells'];pos+=len(r['text'])
                else: seq += [4] if word[pos]=="'" else [alphabet[ord(word[pos])-97]];pos+=1
            with self.subTest(word=word): self.check(seq+[256],word+' ')
        # A six-dot cell used as a one-cell UEB groupsign is instead its
        # Computer8 character, and cannot justify an Appendix contraction.
        self.check([7,0x14,5,7,23,256],'l9klr ')

    def test_removed_categories_cannot_license_longer_shortforms(self):
        # Neither the Appendix search nor the productive decoder may decode
        # removed groupsigns to justify a shortform in a longer word.
        friend=next(r['cells'] for r in ROWS if r['text']=='friend')
        good=next(r['cells'] for r in ROWS if r['text']=='good')
        ness=next(r['cells'] for r in ROWS if r['text']=='ness')
        for seq in (good+ness,friend+ness):
            with self.subTest(seq=seq):
                self.assertEqual(self.observe(seq+[256])['keys'],
                                 self.observe(seq+[256],'computer8')['keys'])

    def test_productive_shortforms_require_letter_sequences(self):
        productive = ('braille', 'great', 'children', 'blind', 'first',
                      'friend', 'good', 'letter', 'little', 'quick')
        for word in productive:
            row = next(r for r in ROWS if r['condition'] == 'SHORT' and r['text'] == word)
            # Digits and non-boundary punctuation are literal Computer8 text,
            # not longer words that may license a productive shortform.
            for suffix in ([2], [12], [60], [4, 1]):
                seq = row['cells'] + suffix + [256]
                with self.subTest(word=word, suffix=suffix):
                    self.assertEqual(self.observe(seq)['keys'],
                                     self.observe(seq, 'computer8')['keys'])
        friend = next(r['cells'] for r in ROWS if r['text'] == 'friend')
        self.check(friend + [14, 256], 'friends ')
        self.check(friend + [14, 19, 10, 15, 256], 'friendship ')
        self.check(friend + [4, 14, 256], "friend's ")
        # Eight-dot capitals in suffixes still obey the no-following-vowel
        # condition; they must not be silently folded or treated as consonants.
        for vowel in (0x41, 0x51):
            seq = friend + [vowel, 256]
            self.assertEqual(self.observe(seq)['keys'], self.observe(seq, 'computer8')['keys'])

    def test_buffer_preview_resets_and_limits(self):
        self.assertEqual(self.observe([1,3])['keys'],[])
        self.assertEqual(self.observe([1,3])['pending'],[1,3])
        self.check([1,3,128],'about\n')
        self.check([1,3,286],'about\t')
        for mode in ('reset','computer8','ueb1','ueb2'):
            self.assertEqual(self.observe([1,3,mode])['pending'],[])
            self.assertEqual(self.observe([1,3,mode])['keys'],[])
        self.check([1]*32,'a'*32)
        self.assertEqual(self.observe([1]*32+[3])['pending'],[3])
        self.assertEqual(self.observe([1,'preview',3])['keys'],[HOST_KEYS[c] for c in 'about'])
        self.assertEqual(self.observe([1,'preview',9])['keys'],[HOST_KEYS[c] for c in 'according'])
        self.assertEqual(self.observe(['preview',3])['keys'],[HOST_KEYS[c] for c in 'but'])
        self.assertEqual(self.observe([1,3,'preview',0x43])['keys'],[HOST_KEYS['B']])
    def test_override_precedence_and_command_extensions(self):
        self.check(['override','1,3','x',1,3,256],'x ')
        self.check(['override','1,3,9','x',1,3,9,256],'x ')
        self.check(['override','1,257','x',1,257,256],'x ')
        self.check(['override','1,256','x',1,256,256],'x ')
        self.check(['override-middle','1,257','x',53,1,257,53,256],'zxz ')
        self.check(['override-end','1,3','x',1,3,256],'x ')
        self.check(['override-end','1,3','x',1,3,36,53,256],'x-as ')
        self.check(['override-start','1,3','x',53,36,1,3,256],'as-x ')
        for opening,closing,text in ((16,16,'\"x\" '),(0x37,0x3e,'(x) ')):
            self.check(['override-start','1,3','x',opening,1,3,closing,256],text)
            self.assertEqual(self.observe(['override-start','1,3','x',opening,1,'preview',3])['keys'],[HOST_KEYS['x']])
        self.check(['override-start','1,3','x',2,1,3,256],'1ab ')
        self.check(['override-start','1,3','x',53,1,3,256],'zab ')
        self.check(['override','1','x',1,3,256],'about ') # longer factory wins

if __name__=='__main__': unittest.main()
