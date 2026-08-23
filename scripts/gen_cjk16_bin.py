#!/usr/bin/env python3
"""
生成 cjk16_font.bin 二进制字库文件
从 lv_font_cjk16.c 中提取所有字体数据，打包成可在运行时加载的二进制格式。

二进制格式:
  [Header 64 bytes]
    magic:      4B  "CJK1"
    version:    4B  (1)
    total_size: 4B
    sections[7]: 每个 4B offset + 4B size = 56B
      [0] glyph_bitmap
      [1] glyph_dsc
      [2] unicode_list_1
      [3] unicode_list_3
      [4] unicode_list_5
      [5] cmaps
      [6] font_dsc
  [各 section 原始数据，按 4 字节对齐]
"""

import re
import struct
import os
import sys

def extract_hex_array(lines, start_idx):
    """从 start_idx 行开始提取十六进制数组，返回 (bytes, end_idx)"""
    values = []
    i = start_idx
    while i < len(lines):
        line = lines[i].strip()
        i += 1
        # 跳过注释行（纯注释）
        if line.startswith('/*') or line.startswith('*') or line.startswith('//'):
            continue
        # 匹配十六进制值
        for match in re.finditer(r'0x[0-9a-fA-F]{1,2}', line):
            values.append(int(match.group(0), 16))
        if '};' in line:
            break
    return bytes(values), i

def extract_glyph_dsc_array(lines, start_idx):
    """从 start_idx 行开始提取 glyph_dsc 结构体数组，返回 (bytes, end_idx)"""
    entries = []
    i = start_idx
    buf = ""
    while i < len(lines):
        line = lines[i].strip()
        i += 1
        if line.startswith('/*') or line.startswith('*') or line.startswith('//'):
            continue
        buf += line
        if '};' in line:
            break

    # 解析结构体
    pattern = r'\{\s*\.bitmap_index\s*=\s*(\d+)\s*,\s*\.adv_w\s*=\s*(-?\d+)\s*,\s*\.box_w\s*=\s*(\d+)\s*,\s*\.box_h\s*=\s*(\d+)\s*,\s*\.ofs_x\s*=\s*(-?\d+)\s*,\s*\.ofs_y\s*=\s*(-?\d+)\s*\}'
    for match in re.finditer(pattern, buf):
        bitmap_index = int(match.group(1))
        adv_w = int(match.group(2))
        box_w = int(match.group(3))
        box_h = int(match.group(4))
        ofs_x = int(match.group(5))
        ofs_y = int(match.group(6))
        packed = (bitmap_index & 0xFFFFF) | ((adv_w & 0xFFF) << 20)
        entries.append(struct.pack('<I', packed) + struct.pack('<BBbb', box_w, box_h, ofs_x, ofs_y))
    return b''.join(entries), i

def extract_uint16_array(lines, start_idx):
    """从 start_idx 行开始提取 uint16_t 数组，返回 (bytes, end_idx)"""
    values = []
    i = start_idx
    while i < len(lines):
        line = lines[i].strip()
        i += 1
        if line.startswith('/*') or line.startswith('*') or line.startswith('//'):
            continue
        for match in re.finditer(r'0x[0-9a-fA-F]+', line):
            val = int(match.group(0), 16)
            if val <= 0xFFFF:
                values.append(struct.pack('<H', val))
        if '};' in line:
            break
    return b''.join(values), i

def extract_cmap_array(lines, start_idx):
    """从 start_idx 行开始提取 cmaps 结构体数组，返回 (bytes, end_idx)"""
    entries = []
    i = start_idx
    buf = ""
    while i < len(lines):
        line = lines[i].strip()
        i += 1
        if line.startswith('/*') or line.startswith('*') or line.startswith('//'):
            continue
        buf += line
        if '};' in line:
            break

    pattern = r'\{\s*\.range_start\s*=\s*(\d+)\s*,\s*\.range_length\s*=\s*(\d+)\s*,\s*\.glyph_id_start\s*=\s*(\d+)\s*,\s*\.unicode_list\s*=\s*(NULL|unicode_list_\d+)\s*,\s*\.glyph_id_ofs_list\s*=\s*(NULL|[\w_]+)\s*,\s*\.list_length\s*=\s*(\d+)\s*,\s*\.type\s*=\s*(\w+)\s*\}'
    type_map = {
        'LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL': 0,
        'LV_FONT_FMT_TXT_CMAP_SPARSE_FULL': 1,
        'LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY': 2,
        'LV_FONT_FMT_TXT_CMAP_SPARSE_TINY': 3,
    }
    for match in re.finditer(pattern, buf):
        range_start = int(match.group(1))
        range_length = int(match.group(2))
        glyph_id_start = int(match.group(3))
        unicode_list = match.group(4)
        glyph_id_ofs_list = match.group(5)
        list_length = int(match.group(6))
        cmap_type = type_map.get(match.group(7), 0)

        ul_ref = 0xFFFFFFFF
        if unicode_list == 'unicode_list_1':
            ul_ref = 0xFFFF0002
        elif unicode_list == 'unicode_list_3':
            ul_ref = 0xFFFF0003
        elif unicode_list == 'unicode_list_5':
            ul_ref = 0xFFFF0004

        go_ref = 0xFFFFFFFF
        if glyph_id_ofs_list != 'NULL':
            go_ref = 0xFFFF0000

        entries.append(struct.pack('<IHHIIHBB', range_start, range_length,
                                    glyph_id_start, ul_ref, go_ref,
                                    list_length, cmap_type, 0))
    return b''.join(entries), i

def extract_font_dsc(lines, start_idx):
    """提取 font_dsc 结构体"""
    i = start_idx
    buf = ""
    while i < len(lines):
        line = lines[i].strip()
        i += 1
        if line.startswith('/*') or line.startswith('*') or line.startswith('//'):
            continue
        buf += line
        if '};' in line:
            break

    m = re.search(r'\.glyph_bitmap\s*=\s*(\w+)', buf)
    bm_ref = 0xFFFF0000 if m and m.group(1) == 'glyph_bitmap' else 0xFFFFFFFF
    m = re.search(r'\.glyph_dsc\s*=\s*(\w+)', buf)
    gd_ref = 0xFFFF0001 if m and m.group(1) == 'glyph_dsc' else 0xFFFFFFFF
    m = re.search(r'\.cmaps\s*=\s*(\w+)', buf)
    cm_ref = 0xFFFF0005 if m and m.group(1) == 'cmaps' else 0xFFFFFFFF
    kern_dsc_ref = 0xFFFFFFFF

    m = re.search(r'\.kern_scale\s*=\s*(\d+)', buf)
    kern_scale = int(m.group(1)) if m else 0
    m = re.search(r'\.cmap_num\s*=\s*(\d+)', buf)
    cmap_num = int(m.group(1)) if m else 0
    m = re.search(r'\.bpp\s*=\s*(\d+)', buf)
    bpp = int(m.group(1)) if m else 1
    m = re.search(r'\.kern_classes\s*=\s*(\d+)', buf)
    kern_classes = int(m.group(1)) if m else 0

    m = re.search(r'\.bitmap_format\s*=\s*(\w+)', buf)
    bf = 0
    if m:
        bf_str = m.group(1)
        if bf_str == 'LV_FONT_FMT_TXT_PLAIN':
            bf = 0

    cache_ref = 0xFFFFFFFF

    return struct.pack('<IIIIHHBBBxI',
                        bm_ref, gd_ref, cm_ref, kern_dsc_ref,
                        kern_scale, cmap_num, bpp, kern_classes, bf,
                        cache_ref), i


def find_array_start(lines, name):
    """查找数组定义的起始行"""
    for idx, line in enumerate(lines):
        if re.search(r'\b' + re.escape(name) + r'\s*(?:\[\s*\])?\s*=', line):
            return idx
    return -1


def main():
    font_c_path = os.path.join(os.path.dirname(__file__), '..', 'LVGL', 'src', 'font', 'lv_font_cjk16.c')
    output_path = os.path.join(os.path.dirname(__file__), '..', 'cjk16_font.bin')

    if not os.path.exists(font_c_path):
        print(f"Error: {font_c_path} not found")
        sys.exit(1)

    with open(font_c_path, 'r', encoding='utf-8', errors='ignore') as f:
        lines = f.readlines()

    # 1. glyph_bitmap
    idx = find_array_start(lines, 'glyph_bitmap')
    if idx < 0:
        print("Error: glyph_bitmap not found")
        sys.exit(1)
    bitmap_data, _ = extract_hex_array(lines, idx)
    print(f"glyph_bitmap: {len(bitmap_data)} bytes")

    # 2. glyph_dsc
    idx = find_array_start(lines, 'glyph_dsc')
    if idx < 0:
        print("Error: glyph_dsc not found")
        sys.exit(1)
    dsc_data, _ = extract_glyph_dsc_array(lines, idx)
    print(f"glyph_dsc: {len(dsc_data)} bytes ({len(dsc_data)//8} entries)")

    # 3. unicode_list_1
    idx = find_array_start(lines, 'unicode_list_1')
    if idx >= 0:
        ul1_data, _ = extract_uint16_array(lines, idx)
    else:
        ul1_data = b''
    print(f"unicode_list_1: {len(ul1_data)} bytes")

    # 4. unicode_list_3
    idx = find_array_start(lines, 'unicode_list_3')
    if idx >= 0:
        ul3_data, _ = extract_uint16_array(lines, idx)
    else:
        ul3_data = b''
    print(f"unicode_list_3: {len(ul3_data)} bytes")

    # 5. unicode_list_5
    idx = find_array_start(lines, 'unicode_list_5')
    if idx >= 0:
        ul5_data, _ = extract_uint16_array(lines, idx)
    else:
        ul5_data = b''
    print(f"unicode_list_5: {len(ul5_data)} bytes")

    # 6. cmaps
    idx = find_array_start(lines, 'cmaps')
    if idx < 0:
        print("Error: cmaps not found")
        sys.exit(1)
    cmaps_data, _ = extract_cmap_array(lines, idx)
    print(f"cmaps: {len(cmaps_data)} bytes ({len(cmaps_data)//20} entries)")

    # 7. font_dsc
    idx = find_array_start(lines, 'font_dsc')
    if idx < 0:
        print("Error: font_dsc not found")
        sys.exit(1)
    font_dsc_data, _ = extract_font_dsc(lines, idx)
    print(f"font_dsc: {len(font_dsc_data)} bytes")

    # 构建 sections
    sections = [
        bitmap_data,
        dsc_data,
        ul1_data,
        ul3_data,
        ul5_data,
        cmaps_data,
        font_dsc_data,
    ]

    # 计算偏移量
    header_size = 4 + 4 + 4 + 7 * (4 + 4)
    offsets = []
    current_offset = header_size
    for data in sections:
        offsets.append(current_offset)
        current_offset += len(data)
        if current_offset % 4 != 0:
            current_offset += 4 - (current_offset % 4)

    total_size = current_offset

    # 写入
    with open(output_path, 'wb') as f:
        f.write(b'CJK1')
        f.write(struct.pack('<I', 1))
        f.write(struct.pack('<I', total_size))
        for i, data in enumerate(sections):
            f.write(struct.pack('<I', offsets[i]))
            f.write(struct.pack('<I', len(data)))
        # pad to header_size
        pos = f.tell()
        if pos < header_size:
            f.write(b'\x00' * (header_size - pos))

        for data in sections:
            f.write(data)
            pad = (4 - (len(data) % 4)) % 4
            if pad:
                f.write(b'\x00' * pad)

    print(f"\nOutput: {output_path}")
    print(f"Total size: {total_size} bytes ({total_size/1024:.1f} KB)")

if __name__ == '__main__':
    main()