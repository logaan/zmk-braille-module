from pathlib import Path
import importlib.util,re,json
from reportlab.pdfgen import canvas
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
pdfmetrics.registerFont(TTFont("Atkinson", "/Users/logaan/Library/Fonts/AtkinsonHyperlegible-Regular.ttf"))
pdfmetrics.registerFont(TTFont("Atkinson-Bold", "/Users/logaan/Library/Fonts/AtkinsonHyperlegible-Bold.ttf"))
from reportlab.lib.colors import HexColor
from reportlab.lib.pagesizes import A4,landscape
from reportlab.lib.utils import simpleSplit
ROOT=Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('audit',ROOT/'scripts/audit-computer8-shorthand.py');a=importlib.util.module_from_spec(spec);spec.loader.exec_module(a)
t=a.patterns();rows=a.audit()['mappings'];alpha=[r for r in rows if r['condition']=='ALPHA'];short=[r for r in rows if r['condition']=='SHORT'];assert len(alpha)==23 and len(short)==75
OUT=ROOT/'output/pdf/eight-dot-shorthand-trifold.pdf';OUT.parent.mkdir(parents=True,exist_ok=True)
W,H=landscape(A4);P=W/3;M=18;CW=P-2*M
c=canvas.Canvas(str(OUT),pagesize=(W,H));c.setTitle('8-dot shorthand | Complete trifold reference');c.setAuthor('Braille module')
INK='#172E37';TEAL='#006B68';MUTED='#52676D';LINE='#D4DEDC';PALE='#F0F5F2';ACCENT='#D9EE9F'
def text(x,y,s,size=9,bold=False,color=INK):
 c.setFillColor(HexColor(color));c.setFont('Atkinson-Bold' if bold else 'Atkinson',size);c.drawString(x,y,s)
def cell(x,y,n,scale=.9):
 # y is baseline; four rows read top to bottom, left 1/2/3/7, right 4/5/6/8.
 for col,dots in enumerate(((1,2,3,7),(4,5,6,8))):
  for row,d in enumerate(dots):
   on=bool(n&(1<<(d-1)));c.setFillColor(HexColor(INK if on else '#CED9D5'))
   c.circle(x+col*3.6*scale,y+(8-row*3.1)*scale,(1.05 if on else .32)*scale,fill=1,stroke=0)
def seq(x,y,ns):
 for i,n in enumerate(ns):cell(x+i*10,y,n)
def mnemonic(x,y,ns):
 for n in ns:
  v=t[n]
  if isinstance(v,int) and chr(v).isalpha():text(x,y,chr(v),9);x+=6
  else:cell(x,y,n,.8);x+=8
 return x

def page(side):
 c.setFillColor(HexColor('#FFFFFF'));c.rect(0,0,W,H,fill=1,stroke=0)
 for i in (1,2):
  c.setStrokeColor(HexColor(LINE));c.setDash(2,4);c.line(P*i,12,P*i,H-12);c.setDash()
 for i in range(3):
  text(P*i+M,14,side+'  /  '+str(i+1),6.5,color=MUTED)
 text(W-176,14,'A4 landscape · duplex, short-edge flip',6.5,color=MUTED)
def panel(i,title,kicker):
 x=i*P+M;text(x,H-28,kicker.upper(),7,True,TEAL);text(x,H-48,title,14,True)
 c.setStrokeColor(HexColor(LINE));c.setLineWidth(.5);c.line(x,H-59,x+CW,H-59)
 return x,H-83
def heading(x,y,s):
 text(x,y+3,s,9.5,True,TEAL);return y-16
COLS=(0,92,163,CW);RH=12

def header(x,y,labels,color=TEAL):
 c.setFillColor(HexColor(color));c.rect(x,y-4,CW,14,fill=1,stroke=0)
 for j,label in enumerate(labels):
  c.setFont('Atkinson-Bold',7.5);c.setFillColor(HexColor('#FFFFFF'))
  c.drawCentredString(x+(COLS[j]+COLS[j+1])/2,y,label)
 c.setStrokeColor(HexColor('#000000'));c.setLineWidth(.65);c.rect(x,y-4,CW,14,fill=0,stroke=1)
 for z in COLS[1:-1]:c.line(x+z,y-4,x+z,y+10)
 return y-15
def para(x,y,s,size=8.2):
 for line in simpleSplit(s,'Atkinson',size,CW):text(x,y,line,size);y-=size+3
 return y-5
def line(x,y,n):
 c.setFillColor(HexColor(PALE if n%2==0 else '#FFFFFF'));c.rect(x,y-3,CW,RH,fill=1,stroke=0)
 c.setStrokeColor(HexColor(LINE));c.setLineWidth(.25);c.line(x,y-3,x+CW,y-3)
 c.setStrokeColor(HexColor('#000000'));c.setLineWidth(.65)
 for z in COLS:c.line(x+z,y-3,x+z,y-3+RH)
 c.setLineWidth(.35);c.line(x,y-3,x+CW,y-3)
def contraction(x,y,rs):
 y=header(x,y,('Output','Input','Mnemonic'),'#2B5D8F')
 for i,r in enumerate(rs):
  line(x,y,i);text(x+4,y,r['contraction'],9);seq(x+129-(len(r['cells'])*10-6)/2,y,r['cells']);mnemonic(x+171,y,r['cells']);y-=12
 return y-6
page('OUTSIDE')
x,y=panel(0,'Digits & symbols','Literal Computer8')
y=heading(x,y,'Digits');COLS=(0,32,CW/2,CW/2+32,CW);RH=14;y=header(x,y,('Number','Input','Number','Input'))
chars={chr(v):i for i,v in enumerate(t) if isinstance(v,int)}
for i,k in enumerate('0123456789'):
 col=i%2;row=i//2;xx=x+col*CW/2
 if col==0:line(x,y-row*14,row)
 text(xx+5,y-row*14,k,10,True);cell(xx+48,y-row*14,chars[k])
y-=5*14;y=heading(x,y,'Punctuation & symbols');COLS=(0,32,65,CW);RH=11.1;y=header(x,y,('Symbol','Input','Name'))
symbols=sorted(k for k in chars if not k.isalnum());assert len(symbols)==32
names={'"':'double quote',"'":'apostrophe','`':'backtick','\\':'backslash','|':'pipe','_':'underscore'}
for i,k in enumerate(symbols):
 line(x,y,i);text(x+5,y,k,10,True);cell(x+46,y,chars[k]);text(x+72,y,names.get(k,''),8,color=MUTED);y-=11.1
assert y>15
x,y=panel(1,'Commands & use','Factory bindings')
y=heading(x,y,'Navigation, editing & modifiers');COLS=(0,140,CW);RH=14;y=header(x,y,('Output','Input'))
keys={'SPACE':'Space','LEFT_ARROW':'Left','RIGHT_ARROW':'Right','UP_ARROW':'Up','DOWN_ARROW':'Down','HOME':'Home','END':'End','PAGE_UP':'Page up','PAGE_DOWN':'Page down','TAB':'Tab','RETURN':'Return','ESC':'Escape','BSPC':'Backspace','LSHFT':'Shift (one-shot)','LCTRL':'Control (one-shot)','LALT':'Option / Alt (one-shot)','LGUI':'Command (one-shot)','GLOBE':'Globe (one-shot)'}
cmds=re.findall(r'BRAILLE_(?:KP|SK)_RULE\((0x[0-9a-f]+), (\w+)',(ROOT/'src/braille_rules_nav.c').read_text());assert len(cmds)==22
for i,(mask,key) in enumerate(cmds+ [('0x13f','Settings')]):
 line(x,y,i);text(x+4,y,keys.get(key,key),8.5);n=int(mask,16)
 if n&256:text(x+150,y,'Space'+(' +' if n&255 else ''),8)
 if n&255:cell(x+194,y,n&255)
 y-=14
y-=9
for s in ['Space, Return or Tab commits the buffered word. At 32 cells, a word boundary is forced.','Other commands or changing mode, layer or settings discards the buffer. Conventional keys also clear it.','Modifiers are one-shot. Home / End send host keys. Settings requires the editor feature.','For literal abbreviations, select ordinary 8-dot before typing. Custom overrides can replace these bindings.']:
 y=para(x,y,s,7.8)
assert y>15
x,y=panel(2,'8-dot shorthand','Complete typing reference')
y=para(x,y,'23 alphabetic wordsigns + 75 shortforms. All other UEB2 contraction categories are excluded.',8.5)
y=heading(x,y,'Alphabet & capitals')
COLS=(0,54,123,CW);RH=12;y=header(x,y,('Letter','Lowercase','Capital'))
for i,k in enumerate('abcdefghijklmnopqrstuvwxyz'):
 line(x,y,i);text(x+6,y,k+' / '+k.upper(),10);cell(x+77,y,chars[k]);cell(x+160,y,chars[k.upper()]);y-=12
y-=8;y=para(x,y,'Capitals add the bottom-left dot. Standalone wordsigns expand; capitals stay literal.',8.2)
y=para(x,y,'Press the dark dots of each cell together; enter cells in order. Pale dots show unused positions.',8.2)
y=para(x,y,'Comma, hash and semicolon stay literal. Unassigned patterns output nothing.',8)
assert y>24
c.showPage();page('INSIDE');COLS=(0,94,164,CW);RH=12
x,y=panel(0,'Wordsigns + A','Contractions / 1 of 3');y=heading(x,y,'Alphabetic wordsigns');y=contraction(x,y,alpha);y=heading(x,y,'Shortforms: about - almost');y=contraction(x,y,short[:10]);assert y>15
x,y=panel(1,'Shortforms A - I','Contractions / 2 of 3');y=contraction(x,y,short[10:43]);y=para(x,y,'Wordsigns apply when standing alone. Shortforms follow their contextual rules, including supported longer-word and suffix forms.',8.2);assert y>15
x,y=panel(2,'Shortforms I - Y','Contractions / 3 of 3');y=contraction(x,y,short[43:]);y=para(x,y,'Letters are shown only where cells represent alphabet letters. A miniature cell in the letters column preserves a non-letter component.',8.2);y=para(x,y,'Braille cells are visual references, not embossed tactile braille. Fold along the guides; front cover is the outside right panel.',8.2);assert y>15
c.save()
# Verify every source entry assigned exactly once; characters represented explicitly.
assert len(chars)==94 and len(alpha)+len(short)==98
print(OUT)
