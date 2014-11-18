/*
   Author: Sambuddha Basu
 */
inb(unsigned short port);
void kmain(char *str);
void clear_screen(void);
int priority(char elem);

#define KEYBOARD_STATUS_PORT	0x64
#define KEYBOARD_DATA_PORT	0x60
#define KEY_ESC			1
#define KEY_ENTER		28

inb(unsigned short port)
{
	unsigned char value;
	asm volatile ("inb    %w1, %0" : "=a" (value) : "Nd" (port));
	return value;
}
unsigned int main_program()
{
	int status, data, len, i, curr_pos;
	char *inp_string;
	char *vidptr = (char *)0xb8000;
	clear_screen();
	kmain("Enter string:");
	len = 0;
	curr_pos = 14;
	while(1) {
		status = inb(KEYBOARD_STATUS_PORT);
		if(status & 0x01) {
			data = inb(KEYBOARD_DATA_PORT);
			if(data == 11) {
				inp_string[len] = '0';
				vidptr[curr_pos * 2] = inp_string[len];
				vidptr[(curr_pos * 2) + 1] = 0x07;
				curr_pos++;
				len++;
			}
			else if(data >= 2 && data <= 10) {
				inp_string[len] = data - 1 + '0';
				vidptr[curr_pos * 2] = inp_string[len];
				vidptr[(curr_pos * 2) + 1] = 0x07;
				curr_pos++;
				len++;
			}
			else if(data == 12) {
				inp_string[len] = '-';
				vidptr[curr_pos * 2] = inp_string[len];
				vidptr[(curr_pos * 2) + 1] = 0x07;
				curr_pos++;
				len++;
			}
			else if(data == 53) {
				inp_string[len] = '/';
				vidptr[curr_pos * 2] = inp_string[len];
				vidptr[(curr_pos * 2) + 1] = 0x07;
				curr_pos++;
				len++;
			}
			else if(data == 14) {
				curr_pos--;
				len--;
				vidptr[curr_pos * 2] = ' ';
				vidptr[(curr_pos * 2) + 1] = 0x07;
			}
			if(data == 42 || data == 54) {
				while((status = inb(KEYBOARD_STATUS_PORT)) & 0x01) {
					data = inb(KEYBOARD_DATA_PORT);
					if(data == 9) {
						inp_string[len] = '*';
						vidptr[curr_pos * 2] = inp_string[len];
						vidptr[(curr_pos * 2) + 1] = 0x07;
						curr_pos++;
						len++;
					}
					else if(data == 13) {
						inp_string[len] = '+';
						vidptr[curr_pos * 2] = inp_string[len];
						vidptr[(curr_pos * 2) + 1] = 0x07;
						curr_pos++;
						len++;
					}
				}
			}
			else if(data == KEY_ENTER)
				break;
		}
	}
	inp_string[len] = '\0';
	clear_screen();
	kmain(inp_string);
	char postfix[100], store[100];
	len = 0;
	i = 0;
	curr_pos = -1;
	while(inp_string[len] != '\0') {
		if(inp_string[len] >= '0' && inp_string[len] <= '9') {
			postfix[i++] = inp_string[len];
		}
		else {
			while(curr_pos > -1 && priority(store[curr_pos]) >= priority(inp_string[len])) {
				postfix[i] = store[curr_pos];
				i++;
				curr_pos--;
			}
			store[++curr_pos] = inp_string[len];
		}
		len++;
	}
	while(curr_pos > -1) {
		postfix[i] = store[curr_pos];
		i++;
		curr_pos--;
	}
	postfix[i] = '\0';
	clear_screen();
	kmain(postfix);
	int stack[100], num1, num2;
	len = 0;
	curr_pos = -1;
	while(postfix[len] != '\0') {
		if(postfix[len] >= '0' && postfix[len] <= '9') {
			stack[++curr_pos] = postfix[len] - '0';
		}
		else {
			num2 = stack[curr_pos--];
			num1 = stack[curr_pos--];
			if(postfix[len] == '+') {
				stack[++curr_pos] = num1 + num2;
			}
			else if(postfix[len] == '-') {
				stack[++curr_pos] = num1 - num2;
			}
			else if(postfix[len] == '*') {
				stack[++curr_pos] = num1 * num2;
			}
			else if(postfix[len] == '/') {
				stack[++curr_pos] = num1 / num2;
			}
		}
		len++;
	}
	if(stack[0] >= 1000000000)
		len = 10;
	else if(stack[0] >= 100000000)
		len = 9;
	else if(stack[0] >= 10000000)
		len = 8;
	else if(stack[0] >= 1000000)
		len = 7;
	else if(stack[0] >= 100000)
		len = 6;
	else if(stack[0] >= 10000)
		len = 5;
	else if(stack[0] >= 1000)
		len = 4;
	else if(stack[0] >= 100)
		len = 3;
	else if(stack[0] >= 10)
		len = 2;
	else
		len = 1;
	clear_screen();
	while(len != 0) {
		vidptr[(len * 2) - 1] = 0x07;
		len--;
		vidptr[(len * 2)] = (stack[0] % 10) + '0';
		stack[0] /= 10;
	}
	return 0;
}
int priority(char elem) {
	if(elem == '+' || elem == '-')
		return 0;
	else if(elem == '*' || elem == '/')
		return 1;
}
void kmain(char *str)
{
	char *vidptr = (char *)0xb8000; 	//video mem begins here.
	unsigned int i = 0;
	unsigned int j = 0;
	while(str[j] != '\0') {
		vidptr[i] = str[j];
		vidptr[i+1] = 0x07;
		++j;
		i = i + 2;
	}
	return;
}
void clear_screen(void) {
	char *vidptr = (char *)0xb8000;
	unsigned int i = 0;
	while(i < 80 * 25 * 2) {
		vidptr[i] = ' ';
		vidptr[i + 1] = 0x07;
		i += 2;
	}
}
