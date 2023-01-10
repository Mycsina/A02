#
# Tomás Oliveira e Silva, AED, November 2022
#
# makefile to compile the A.02 assignment (word ladder)
#

clean:
	rm -rf a.out word_ladder *.exe

word_ladder: word_ladder.cpp
	g++ -Wall -Wextra -O3 word_ladder.cpp -o word_ladder -lm -march=native

stats: word_ladder.cpp
	g++ -Wall -Wextra -O3 word_ladder.cpp -o word_ladder -lm -march=native -D_stats_

detail: word_ladder.cpp
	g++ -Wall -Wextra -O3 word_ladder.cpp -o word_ladder -lm -march=native -D_detail_

full: word_ladder.cpp
	g++ -Wall -Wextra -O3 word_ladder.cpp -o word_ladder -lm -march=native -D_full_

debug: word_ladder.cpp
	g++ -Wall -Wextra -O0 -ggdb3 word_ladder.cpp -o word_ladder -lm -march=native -D_detail_