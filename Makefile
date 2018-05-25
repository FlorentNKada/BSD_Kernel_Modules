KMOD=packet_displayer
SRCS=packet_displayer.c

rclean:
	@make clean
	rm -f *.o

.include <bsd.kmod.mk>
