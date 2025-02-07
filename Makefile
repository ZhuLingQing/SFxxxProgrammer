DEBUG?=OFF

simu:
	if [ ! -d build/sim ] ; then mkdir build/sim -p ; fi
	cd build/sim;  \
	cmake \
		-G "Unix Makefiles" \
		-DDEBUG=$(DEBUG) \
		-DPROJECT=sim\
		../..; \
	make -j;
	@echo "Simulation build done"

test: simu
	cd build/sim; \
	ctest
	@echo "Simulation test done"

format:
	@python3 cicd/run_clang_format.py -i -r umd sim --exclude third-party/

clean:
	@rm -rf build
	@rm -rf *.run