#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""ICEB mapping coverage and shared-engine contracted reading regressions."""
import json
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest
from test_computer8 import HOST_KEYS
HOST_KEYS = dict(HOST_KEYS, **{"\n": (7 << 16) | 40, "\t": (7 << 16) | 43})
from test_ueb1 import SOURCES, ROOT

ALPHABET = "⠁⠃⠉⠙⠑⠋⠛⠓⠊⠚⠅⠇⠍⠝⠕⠏⠟⠗⠎⠞⠥⠧⠺⠭⠽⠵"
def cells(s):
    return [256 if c==' ' else ord(c)-0x2800 for c in s]
def literal(s):
    return [ord(ALPHABET[ord(c)-97])-0x2800 for c in s]

class Grade2(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp=tempfile.TemporaryDirectory();cls.addClassCleanup(cls.tmp.cleanup)
        cls.binary=Path(cls.tmp.name)/'grade2'
        sources=['tests/host/test_grade2.c',*SOURCES[1:]]
        subprocess.run(shlex.split(os.environ.get('CC','cc'))+[
            '-std=c11','-Wall','-Wextra','-Werror','-DCONFIG_ZMK_SPLIT=1',
            '-DCONFIG_ZMK_BRAILLE_GRADE2=1','-I',str(ROOT/'tests/host/stubs'),
            '-I',str(ROOT/'include'),*[str(ROOT/s) for s in sources],'-o',str(cls.binary)],check=True)
    def observe(self, seq,mode='ueb2'):
        return json.loads(subprocess.check_output([str(self.binary),mode,*map(str,seq)],text=True))
    def check(self,seq,text):
        actual=self.observe(seq)
        self.assertEqual(actual['keys'],[HOST_KEYS[c] for c in text])
        self.assertEqual(actual['pending'],[])
    def test_all_182_contractions(self):
        rows=json.loads((ROOT/'references/research/ueb-grade2-contractions.yaml').read_text().split('\n',2)[2])['contractions']
        self.assertEqual(len(rows),182)
        for r in rows:
            seq=r['cells']; text=r['text']; kind=r['condition']
            if kind in ('ANY','MIDDLE'):seq=[1]+seq+[1];text='a'+text+'a'
            if kind=='START':seq=seq+[1];text=text+'a'
            if kind=='FINAL':seq=[1]+seq;text='a'+text
            with self.subTest(word=r['text'],condition=kind):
                self.check(seq+[256],text+' ')
                self.check([32,32]+seq+[256],text.upper()+' ')
                self.check([32]+seq+[256],text[0].upper()+text[1:]+' ')
                self.assertEqual(self.observe([48,48]+seq+[256])['keys'],
                                 self.observe([48,48]+seq+[256],mode='ueb1')['keys'])
    def test_whole_word_buffer_and_boundaries(self):
        self.assertEqual(self.observe([3])['keys'],[])
        self.assertEqual(self.observe([3])['pending'],[3])
        self.check([3,256],'but ')
        self.check([3,128],'but\n')
        self.check([3,286],'but\t')
        self.assertEqual(self.observe([3,257])['pending'],[])
        self.assertNotIn(HOST_KEYS['b'],self.observe([3,257])['keys'])
        self.assertEqual(self.observe([3,50])['keys'],[])
        self.check([3,50,256],'but. ')
        self.check([1]*32,'a'*32)
        result=self.observe([1]*32+[3]);self.assertEqual(result['pending'],[3])
        self.assertEqual(len(result['keys']),32)
        for reset in ('reset','ueb1','computer8'):
            self.assertEqual(self.observe([3,reset])['keys'],[])
            self.assertEqual(self.observe([3,reset])['pending'],[])
    def test_indicators(self):
        for b,t in [('⠠⠃ ','But '),('⠠⠠⠃ ','BUT '),('⠠⠠⠠⠃ ⠉⠠⠄ ','BUT CAN '),
                    ('⠰⠃ ','b '),('⠰⠰⠃⠉ ','bc '),('⠰⠰⠰⠃ ⠉⠰⠄ ⠙ ','b c do '),
                    ('⠼⠁⠃⠉ ','123 '),('⠼⠁⠅⠃ ','1kb '),('⠼⠁⠤⠃ ','1-but '),
                    ('⠼⠁⠰⠄⠃ ','1b '),('⠁⠰⠑ ','aence ')]:
            with self.subTest(b=b):self.check(cells(b),t)
    def test_reading_positions_and_shortforms(self):
        examples=[('⠨⠆⠃ ','but '),('⠨⠶⠃⠨⠄ ','but '),('⠃⠁ ','ba '),('⠉⠄⠞ ','can\'t '),('⠆⠁ ','bea '),('⠁⠆⠁ ','abba '),
                  ('⠁⠆ ','a; '),('⠆⠂ ',';, '),('⠁⠃ ','about '),('⠁⠃⠎ ','abs '),
                  ('⠁⠇⠍⠎ ','alms '),('⠓⠍⠎ ','hms '),('⠋⠗⠎ ','friends '),
                  ('⠁⠃⠧⠛⠗⠨⠙ ','aboveground '),('⠗⠑⠏⠙ ','repaid '),
                  ('⠛⠙⠁⠋⠝ ','goodafternoon '),('⠃⠗⠇⠞⠞⠑ ','braillette '),
                  ('⠟⠅⠎⠃⠥⠗⠛ ','quicksburg '),('⠠⠙⠑⠁⠋⠠⠃⠇ ','DeafBlind '),
                  ('⠙⠤⠭⠤⠽⠗⠋⠑⠗ ','do-it-yourselfer '),('⠰⠠⠃⠇⠧⠙ ','Blvd '),
                  ('⠰⠠⠠⠇⠇⠉ ','LLC '),('⠰⠰⠠⠙⠕⠃⠗⠇⠚⠊⠝ ','Dobrljin '),('⠇⠔⠅⠇⠗ ','linklr '),
                  ('⠠⠽ ⠩⠙ ⠗⠉⠧ ⠽⠗ ⠇⠗ ⠞⠍ ⠁⠋⠝⠲ ',
                   'You should receive your letter tomorrow afternoon. ')]
        for b,t in examples:
            with self.subTest(b=b):self.check(cells(b),t)
    def test_longer_words_with_literal_suffixes(self):
        rows=json.loads((ROOT/'references/research/ueb-grade2-contractions.yaml').read_text().split('\n',2)[2])['contractions']
        shorts=sorted((r for r in rows if r['condition']=='SHORT'),key=lambda r:-len(r['text']))
        words=json.loads((ROOT/'references/research/ueb-grade2-shortforms.json').read_text())['longer_words']
        for word in words:
            if any(c.isupper() for c in word) or '-' in word:continue
            seq=[];pos=0
            while pos<len(word):
                match=next((r for r in shorts if word.startswith(r['text'],pos)),None)
                if match:seq+=match['cells'];pos+=len(match['text'])
                else:seq+=[4] if word[pos]=="'" else literal(word[pos]);pos+=1
            with self.subTest(word=word):self.check(seq+[256],word+' ')
    def test_preview_tracks_current_held_contribution(self):
        self.assertEqual(self.observe([1,'preview',3])['keys'],[HOST_KEYS[c] for c in 'about'])
        self.assertEqual(self.observe([1,'preview',9])['keys'],[HOST_KEYS[c] for c in 'according'])
        self.assertEqual(self.observe([1,3,'preview',9])['keys'],[HOST_KEYS['c']])
        self.assertEqual(self.observe([48,1,'preview',3])['keys'],[HOST_KEYS['b']])
        self.assertEqual(self.observe([32,32,1,'preview',3])['keys'],[HOST_KEYS[c] for c in 'ABOUT'])
    def test_override_precedence(self):
        self.check(['override',48,'x',48,3,256],'xb ')
        self.check(['override','3,9','z',3,9,256],'z ')
        self.check(['override',1,'z',1,3,256],'about ')
        self.assertEqual(self.observe(['override',48,'x','preview',48])['keys'],[HOST_KEYS['x']])
    def test_appendix_data(self):
        data=json.loads((ROOT/'references/research/ueb-grade2-shortforms.json').read_text())
        self.assertEqual(len(data['longer_words']),328)
        self.assertIn('DeafBlind',data['longer_words'])

if __name__=='__main__':unittest.main()
