from PIL import Image, ImageDraw, ImageFont

def char_to_matrix(char, font_path, font_size):
    # Create a blank image
    image = Image.new("1", (font_size, font_size), color=0)

    # Get a drawing context
    draw = ImageDraw.Draw(image)

    # Load a font
    font = ImageFont.truetype(font_path, font_size)

    # Draw the character in white on the black background
    draw.text((0, 0), char, font=font, fill=1)

    # Convert the image to a pixel matrix
    pixel_matrix = list(image.getdata())
    pixel_matrix = [pixel_matrix[i:i + font_size] for i in range(0, len(pixel_matrix), font_size)]

    return pixel_matrix

def print_matrix(matrix):
    # Display the matrix
    for row in matrix:
        print(" ".join(map(str, row)))

# Example usage

font_path = "/content/DejavuSansMonoBold-6Zmo.ttf"  # Replace with your font file path
font_size = 16  # Adjust the font size as needed

def analyse_matrix(matrix, code=False): # draws by default
  byte = [0]*12
  if(code):
    print("{", end='')
  for i in range(3,15):      # 12 rows
    thisstring = ""
    for j in range(1,9):     # 8 columns
        thisstring = thisstring + str(matrix[i][j])
    byte = int(thisstring,2)
    if(code):
      print(f"{byte:#0{4}x}",end='') # for padding (https://stackoverflow.com/questions/12638408/decorating-hex-function-to-pad-zeros)
      print("," if (i != 14) else "},", end='')
    else:
      print(thisstring)

for char_index in range(ord('0'), ord('9') + 1):
    character = chr(char_index)
    matrix_representation = char_to_matrix(character, font_path, font_size)
    if matrix_representation:
        analyse_matrix(matrix_representation, code=True)
        print(f"\t\t// char '{character}' '({char_index})'")
        # print_matrix(matrix_representation)
for char_index in range(ord('A'), ord('Z') + 1):
    character = chr(char_index)
    matrix_representation = char_to_matrix(character, font_path, font_size)
    if matrix_representation:
        analyse_matrix(matrix_representation, code=True)
        print(f"\t\t// char '{character}' '({char_index})'")
        # print_matrix(matrix_representation)
for char_index in range(ord('.'), ord('.') + 1):
    character = chr(char_index)
    matrix_representation = char_to_matrix(character, font_path, font_size)
    if matrix_representation:
        analyse_matrix(matrix_representation, code=True)
        print(f"\t\t// char '{character}' '({char_index})'")
        # print_matrix(matrix_representation)