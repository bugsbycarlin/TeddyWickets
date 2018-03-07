.PHONY: TeddyWickets
.PHONY: ModelViewer
.PHONY: GLTest

TeddyWickets:
		g++ -w \
		./Source/main.cpp \
		./Source/point.cpp \
		./Source/colors.cpp \
		./Source/graphics.cpp \
		./Source/globals.cpp \
		./Source/screen.cpp \
		./Source/title.cpp \
		./Source/control_map.cpp \
		./Source/control_setup.cpp \
		./Source/game.cpp \
		./Source/physics.cpp \
		./Source/surface.cpp \
		./Source/character.cpp \
		./Source/model.cpp \
		./Source/bumper.cpp \
		./Source/hazard.cpp \
		./Source/textures.cpp \
		./Source/textbox.cpp \
		./Source/triangle.cpp \
		-std=c++11 \
		-o TeddyWickets \
		-framework OpenGL \
		-framework SDL2 \
		-framework SDL2_image \
		-framework SDL2_ttf \
		-I Source/glm \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libBulletSoftBody_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-I Source/fmod_src \
		./Libraries/libfmod.dylib \
		-rpath ./Libraries

ModelViewer:
		g++ -w \
		./Source/model_viewer_main.cpp \
		./Source/globals.cpp \
		./Source/graphics.cpp \
		./Source/screen.cpp \
		./Source/model_viewer.cpp \
		./Source/model.cpp \
		./Source/point.cpp \
		./Source/triangle.cpp \
		./Source/textures.cpp \
		-std=c++11 \
		-o ModelViewer \
		-framework OpenGL \
		-framework SDL2 \
		-framework SDL2_image \
		-I Source/glm \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-I Source/fmod_src \
		./Libraries/libfmod.dylib \
		-rpath ./Libraries

GLTest:
	  g++ -w \
		./Source/gl_test_main.cpp \
		-std=c++11 \
		-o GLTest \
		-framework OpenGL \
		-framework SDL2 \
		-framework SDL2_image \
		-I Source/glm \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-I Source/fmod_src \
		./Libraries/libfmod.dylib \
		-rpath ./Libraries