.PHONY: TeddyWickets

TeddyWickets:
		g++ -w \
		./Source/main.cpp \
		./Source/globals.cpp \
		./Source/screen.cpp \
		./Source/title.cpp \
		./Source/game.cpp \
		./Source/physics.cpp \
		./Source/point.cpp \
		./Source/surface.cpp \
		./Source/character.cpp \
		./Source/bumper.cpp \
		./Source/wicket.cpp \
		./Source/textures.cpp \
		-std=c++11 \
		-o TeddyWickets \
		-framework OpenGL \
		-framework SDL2 \
		-framework SDL2_image \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-I Source/fmod_src \
		./Libraries/libfmod.dylib \
		-rpath ./Libraries