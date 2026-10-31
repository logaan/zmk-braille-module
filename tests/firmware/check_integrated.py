#!/usr/bin/env python3
"""Validate final display/editor firmware artifacts; never flashes hardware."""
import argparse
from pathlib import Path
import re
from check_smoke import check_keymap

def check(build, keyboard):
    config=set((build/'zephyr/.config').read_text().splitlines())
    ninja=(build/'build.ninja').read_text()
    header=build/'zephyr/include/generated/zephyr/devicetree_generated.h'
    if not header.exists():header=build/'zephyr/include/generated/devicetree_generated.h'
    check_keymap(header.read_text(),keyboard)
    central='CONFIG_ZMK_FEATURE_BRAILLE=y' in config
    if central:
        for feature in ('ZMK_BRAILLE_GRADE2','ZMK_BRAILLE_COMPUTER8_SHORT','ZMK_BRAILLE_STORAGE','ZMK_BRAILLE_SETTINGS_UI'):
            assert f'CONFIG_{feature}=y' in config,feature
        stack=next(int(s.split('=')[1]) for s in config if s.startswith('CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE='))
        assert stack>=6144,('system workqueue stack',stack)
        linked=(build/'zephyr/zmk.map').read_text()
        for name in ('braille_engine_preview_rule','braille_computer8_short_match','braille_grade2_match','braille_storage_save',
                     'braille_editor_snapshot','braille_tables_select'):
            assert name in linked,name
        for source in ('braille_rules_grade2','braille_storage','braille_editor','braille_editor_zmk'):
            assert source+'.c.obj:' in ninja,source
        if 'CONFIG_SHIELD_TOUCAN_DONGLE=y' in config:
            assert 'CONFIG_SHIELD_TOUCAN_BRAILLE_UI=y' in config
            assert re.search(r'^build .*toucan_braille_ui.*screen\.c\.obj:',ninja,re.M)
            assert not re.search(r'^build .*prospector_adapter.*(?:custom_status_screen|layouts/).*\.obj:',ninja,re.M)
        else:
            assert 'CONFIG_ZMK_BRAILLE_WIDGET=y' in config
            assert 'braille_widget.c.obj:' in ninja
    else:
        assert 'braille_engine.c.obj:' not in ninja
    assert (build/'zephyr/zmk.uf2').stat().st_size>0
    print(f'{build}: integrated {keyboard} '+('central modes/storage/editor/display' if central else 'peripheral')+' PASS')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--keyboard',choices=('qbf','toucan'),required=True);p.add_argument('builds',nargs='+',type=Path)
    a=p.parse_args()
    for b in a.builds:check(b,a.keyboard)
