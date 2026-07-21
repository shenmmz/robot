import os

dir_path = r'c:\Users\31422\Desktop\Robot_R06\src\vision\cs_vmcontrol'

for filename in os.listdir(dir_path):
    if filename.endswith('.h') or filename.endswith('.cpp'):
        filepath = os.path.join(dir_path, filename)
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        with open(filepath, 'w', encoding='gbk', errors='ignore') as f:
            f.write(content)
        print(f'Converted to GBK: {filename}')

print('Done!')
