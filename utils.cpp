#include "utils.h"

UTILS* UTILS::foo(nullptr);

UTILS::UTILS(QWidget *parent) : QWidget(parent) {
	foo = this;
}

void UTILS::open_file() {
	QString filename = QFileDialog::getOpenFileName(nullptr, tr("Open File"), "./");
	emit _open_file(filename);
}

void UTILS::save_file() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save File"), "./");
	emit _save_file(filename);
}

void UTILS::logd(const char* s) {
	foo->_logd(s);
}

void UTILS::alert(const char* s) {
	QMessageBox::warning(nullptr, "Alert", s);
}

string UTILS::int_to_string(int num) {
	stringstream ss;
	ss << num;
	string s;
	ss >> s;
	return s;
}

void UTILS::uv_map(int cube_index_row, int cube_index_col, 
	double &bl_u, double &bl_v, double &size) {

	// 当纹理大小发生改变时，只修改size值即可
	const static double SIZE = 0.25;
	size = SIZE;
	bl_u = cube_index_col * SIZE;
	bl_v = 1 - cube_index_row * SIZE - SIZE;
}

string UTILS::get_cube_name(int cube_type) {
	static const string cube_name[] = {
		"sun",		"cloud",		"water",
		"air",		"earth",		"stone",
		"grass",	"tree",			"leaf"
	};
	const int bias = 3;
	return cube_name[cube_type + bias];
}






////////////////////////////////////////////////////////////////////////
////////////////////////// FILE SYSTEM /////////////////////////////////
////////////////////////////////////////////////////////////////////////
void UTILS::save(const char* file) {
	string test_str = file;
	if (!file || test_str == "") {
		alert("Please input a valid file");
		return;
	}
	ofstream output;
	output.open(file);
	int count = 0;
	int last_type = CUBE_AIR, cube_count = 0;
	for (int i = 0; i < MAX_X; i++) {
		for (int j = 0; j < MAX_Y; j++) {
			for (int k = 0; k < MAX_Z; k++) {
				if(mapData[i][j][k] == last_type) cube_count++;
				else {
					output << cube_count << " " << last_type << " ";
					last_type = mapData[i][j][k];
					cube_count = 1;
				}
			}
		}
	}
	output << "-1 -1"; // magic number
	output << '\n';
	output << cameraPos.x << ' ' << cameraPos.y << ' ' << cameraPos.z << '\n';
	output << cameraFront.x << ' ' << cameraFront.y << ' ' << cameraFront.z << '\n';
	output << cameraUp.x << ' ' << cameraUp.y << ' ' << cameraUp.z << '\n';
	output << gameState << '\n';
	output << cameraForwardAngle << '\n';
	output << cameraSpeed << '\n';
	output << cameraYaw << '\n';
	output << cameraPitch << '\n';
	output.close();
}

void UTILS::load(const char* file) {
	string test_str = file;
	if (!file || test_str == "") {
		alert("Please choose a valid file");
		return;
	}
	ifstream input;
	input.open(file);
	int i = 0, j = 0, k = 0;
	int cube_count = 0, type = CUBE_AIR;
	while (i < MAX_X) {
		input >> cube_count >> type;
		if (cube_count == -1 && type == -1) break;
		while (cube_count--) {
			mapData[i][j][k] = type;
			k++;
			if (k >= MAX_Z) {
				k = 0;
				j++;
				if (j >= MAX_Y) {
					j = 0;
					i++;
				}
			}
		}
	}
	float x, y, z;
	input >> x >> y >> z;
	cameraPos = glm::vec3(x, y, z);
	input >> x >> y >> z;
	cameraFront = glm::vec3(x, y, z);
	input >> x >> y >> z;
	cameraUp = glm::vec3(x, y, z);
	input >> gameState;
	input >> cameraForwardAngle;
	input >> cameraSpeed;
	input >> cameraYaw;
	input >> cameraPitch;
	input.close();
}

void UTILS::clear(const char* file) {
	ofstream output;
	output.open(file);
	output << "";
	output.close();
}