.PHONY: TeddyWickets
.PHONY: ModelViewer
.PHONY: GLTest
.PHONY: CollisionIssue
.PHONY: CollisionIssueStep2
.PHONY: CollisionIssueStep3

TeddyWickets:
		g++ -w \
		./Source/main.cpp \
		./Source/point.cpp \
		./Source/colors.cpp \
		./Source/graphics.cpp \
		./Source/hot_config.cpp \
		./Source/globals.cpp \
		./Source/screen.cpp \
		./Source/title.cpp \
		./Source/control_map.cpp \
		./Source/control_setup.cpp \
		./Source/bear_select.cpp \
		./Source/game.cpp \
		./Source/tinyxml_src/tinyxml2.cpp \
		./Source/editor.cpp \
		./Source/physics.cpp \
		./Source/surface.cpp \
		./Source/character.cpp \
		./Source/model.cpp \
		./Source/model_cache.cpp \
		./Source/bumper.cpp \
		./Source/hazard.cpp \
		./Source/wicket.cpp \
		./Source/free_star.cpp \
		./Source/boxguy.cpp \
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
		-I Source/tinyxml_src \
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


CollisionIssue:
	  g++ -w \
		./Source/collision_issue.cpp \
		-std=c++11 \
		-o CollisionIssue \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libBulletSoftBody_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-rpath ./Libraries


CollisionIssueStep2:
	  g++ -w \
		./Source/collision_issue_step_2.cpp \
		-std=c++11 \
		-o CollisionIssueStep2 \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libBulletSoftBody_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-rpath ./Libraries


CollisionIssueStep3:
	  g++ -w \
		./Source/collision_issue_step_3.cpp \
		-std=c++11 \
		-o CollisionIssueStep3 \
		-I Source/bullet_src \
		./Libraries/libBulletCollision_gmake_x64_release.a \
		./Libraries/libBulletDynamics_gmake_x64_release.a \
		./Libraries/libBulletSoftBody_gmake_x64_release.a \
		./Libraries/libLinearMath_gmake_x64_release.a \
		-rpath ./Libraries
