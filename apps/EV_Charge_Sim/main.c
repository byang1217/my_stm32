#include "common.h"
#include "version.h"

#define SHELL_UART_ID 1
#define SHELL_UART_SPEED 9600

static int shell_test(int argc, char **argv)
{
	int i;

	pr_log("argc = %d\n", argc);
	for (i = 0; i < argc; i++)
		pr_log("argv[%d]: \"%s\"\n", i, argv[i]);
	return 0;
}

static void onboard_led(int on)
{
	static int init;

	if (!init) {
		bsp_gpio_direction(PC_13, 0);
		init = 1;
	}
	bsp_gpio_set(PC_13, !on);
}
static int shell_onboard_led_toggle(int argc, char **argv)
{
	static char on;
	on = !on;
	onboard_led(on);
	pr_log("led: %s\n", on ? "on" : "off");
	return 0;
}

static int shell_putc(int c)
{
	bsp_uart_putc(SHELL_UART_ID, (char)c);
	return c;
}

static int shell_getc(void)
{
	return bsp_uart_getc(SHELL_UART_ID);
}

void usb_uart_tx_async_handle(void)
{

}

void usb_uart_tx_done(void)
{

}

void usb_uart_rx(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	int i;

	for (i = 0; i < Nb_bytes; i++)
		shell_putc((int)(data_buffer[i]));
}

static int shell_usb_uart(int argc, char **argv)
{
	return 0;
}

static const struct shell_cmd app_cmds[] = {
	{
		.cmd_str = "led",
		.help_str = "led toggle",
		.fn = shell_onboard_led_toggle,
	},
	{
		.cmd_str = "test",
		.help_str = "test",
		.fn = shell_test,
	},
	{
		.cmd_str = "usb_uart",
		.help_str = "usb_uart",
		.fn = shell_usb_uart,
	},
};

struct shell default_shell = {
	.shell_putc = shell_putc,
	.shell_getc = shell_getc,
	.cmds = app_cmds,
	.cmd_num = sizeof(app_cmds)/sizeof(app_cmds[0]),
};

void main(void)
{
	bsp_init();
	bsp_uart_init(SHELL_UART_ID, SHELL_UART_SPEED);
	pr_log("\n%s\n", VERSION_STRING);
	bsp_usb_init();
	for (;;) {
		shell_kick(&default_shell);
	}
}
