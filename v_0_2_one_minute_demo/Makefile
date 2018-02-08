.PHONY: TeddyWickets
.PHONY: ModelViewer

TeddyWickets:
		g++ -w \
		./Source/main.cpp \
		./Source/teddy_gl.cpp \
		./Source/globals.cpp \
		./Source/screen.cpp \
		./Source/title.cpp \
		./Source/control_map.cpp \
		./Source/control_setup.cpp \
		./Source/game.cpp \
		./Source/physics.cpp \
		./Source/point.cpp \
		./Source/surface.cpp \
		./Source/character.cpp \
		./Source/model.cpp \
		./Source/bumper.cpp \
		./Source/wicket.cpp \
		./Source/textures.cpp \
		./Source/textbox.cpp \
		-std=c++11 \
		-o TeddyWickets \
		-framework OpenGL \
		-framework SDL2 \
		-framework SDL2_image \
		-framework SDL2_ttf \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-I Source/fmod_src \
		./Libraries/libfmod.dylib \
		-rpath ./Libraries


ModelViewer:
		g++ -w \
		./Source/model_viewer_main.cpp \
		./Source/globals.cpp \
		./Source/screen.cpp \
		./Source/model_viewer.cpp \
		./Source/model.cpp \
		./Source/point.cpp \
		./Source/textures.cpp \
		-std=c++11 \
		-o ModelViewer \
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