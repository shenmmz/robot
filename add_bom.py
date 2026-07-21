import os

bom = b'\xef\xbb\xbf'
dir_path = r'c:\Users\31422\Desktop\Robot_R06\src\vision\cs_vmcontrol'

for filename in os.listdir(dir_path):
    if filename.endswith('.h') or filename.endswith('.cpp'):
        filepath = os.path.join(dir_path, filename)
        with open(filepath, 'rb') as f:
            content = f.read()
        if not content.startswith(bom):
            with open(filepath, 'wb') as f:
                f.write(bom + content)
            print(f'Added BOM to: {filename}')
        else:
            print(f'Already has BOM: {filename}')

print('Done!')
