import nbformat

# 打开.py文件，并读取其中的代码
with open('jisuan.py', 'r') as f:
    code = f.read()

# 创建一个空白的.ipynb文件
nb = nbformat.v4.new_notebook()

# 添加代码单元格到.ipynb文件中
code_cell = nbformat.v4.new_code_cell(code)
nb['cells'] = [code_cell]

# 保存.ipynb文件
nbformat.write(nb, 'jisuan.ipynb')
