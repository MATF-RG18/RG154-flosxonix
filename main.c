#include <GL/glut.h>
#include <stdbool.h>
#include <stdlib.h>
#define TIMER_ID 0
#define TIMER_INTERVAL 20
#define UNUSED_ARG(x) ((void)(x))

static int win_width, win_height;
static bool animation_ongoing = false;

/* Svi moguci pokreti heroja u igri
 */
typedef enum {
	PASSIVE,
	NORTH,
	EAST,
	SOUTH,
	WEST
} hero_motion; 

/* Informacije o heroju
 */
typedef struct {
	float position_x;
	float position_y;
	float actual_position_x;
	float actual_position_y;
	int size;
	hero_motion direction;
	float velocity;
} hero_info;

/* Informacije o platformi
 */
typedef struct {
	int size_x;
	int size_y;
} field_info;

static hero_info hero;
static field_info field;

static void game_info_initialization();
static void update_actual_position();
static bool collision_detection();

static void draw_hero();
static void draw_field();

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);

static void game_info_initialization()
{
	field.size_x = 20;
	field.size_y = 20;

	hero.velocity = 0.4;
	hero.position_x = (field.size_x - 1) / 2.0;
	hero.position_y = 0;
	update_actual_position();
	hero.size = 1;
	hero.direction = PASSIVE;
}

/* Vrsi se izracunavanje koordinata u prostoru.
 * Pozicija za heroja (0, 0) je u donjem levom uglu 
 */
static void update_actual_position()
{
	hero.actual_position_x = (field.size_x - 1) / 2.0 - hero.position_x;
	hero.actual_position_y = -(field.size_y - 1) / 2.0 + hero.position_y;
}

/* Provera za one timer */
static bool collision_detection()
{
	if(hero.position_x < 0){
		hero.position_x = 0;
		return true;
	}else if(hero.position_x > field.size_x - 1){
		hero.position_x = field.size_x - 1;
		return true;
	}
	if(hero.position_y < 0){
		hero.position_y = 0;
		return true;
	}else if(hero.position_y > field.size_y - 1){
		hero.position_y = field.size_y - 1;
		return true;
	}
	return false;
}

/* Iscrtavanja komponenti iz igrice, podesavanje refleksija svetlosti */
static void draw_hero()
{
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); 
	GLfloat ambient_coeffs[] = {0.7, 0.1, 0.1, 1};
	GLfloat diffuse_coeffs[] = {1, 0, 0, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	update_actual_position();
	glPushMatrix();
		glTranslatef(hero.actual_position_x, hero.actual_position_y, 0);
		glutSolidCube(hero.size);
	glPopMatrix();
}

static void draw_field()
{
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); 
	GLfloat ambient_coeffs1[] = {0.1, 0.1, 0.2, 1};
	GLfloat diffuse_coeffs1[] = {0, 0, 0.7, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs1);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs1);
	glTranslatef(0, 0, 1);
	/* Inicijalan zid */
	glPushMatrix();
		glTranslatef((field.size_x - 1) / 2.0, 0, 0);
		glScalef(1, field.size_y, 1);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, (field.size_y - 1) / 2.0, 0);
		glScalef(field.size_x, 1, 1);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, (-field.size_y + 1) / 2.0, 0);
		glScalef(field.size_x, 1, 1);
		glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
		glTranslatef((-field.size_x + 1)/ 2.0, 0, 0);
		glScalef(1, field.size_y, 1);
		glutSolidCube(1);
	glPopMatrix();
	/* ZBIVANJE TODO*/

	glTranslatef(0, 0, 1);
	GLfloat diffuse_coeffs2[] = {0.5, 0.5, 0.5, 1};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs2);
	glPushMatrix();
		glScalef(field.size_x, field.size_y, 1);
		glutSolidCube(1);
	glPopMatrix();
}

static void on_display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluLookAt(0, -field.size_y, -field.size_y,
		  0, 0, 0,
		  0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);
	/* Svetlo globalno */
	glEnable(GL_LIGHT0); 
	/* Postavlja se jedinicna matrica zbog pozicije direkcionog svetla 
	 * i celokupnog zdravlja iscrtavanja 
	 */
	glLoadIdentity();
	float light_position[] =  {-1, 0, -0.5};
	/* Boje svetla uvek iste */
	float light_ambient[] =  {0.3, 0.3, 0.3, 1};
	float light_diffuse[] =  {0.7, 0.7, 0.7, 1};
	float light_specular[] =  {0.9, 0.9, 0.9, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	GLfloat ambient_coeffs[] = {0.3, 0.1, 0.1, 1};
	GLfloat diffuse_coeffs[] = {1, 0, 0, 1};
	GLfloat specular_coeffs[] = {1, 1, 1, 1};
	GLfloat shininess = 20;
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	glShadeModel(GL_SMOOTH);
	draw_hero();
	draw_field();
	glutSwapBuffers();
}

static void on_keyboard(unsigned char key, int x, int y)
{
	UNUSED_ARG(x);
	UNUSED_ARG(y);
	switch (key) {
		case 27:
			exit(0);
			break;
		case ' ':
			animation_ongoing = !animation_ongoing;
			break;
		case 'w':
		case 'W':
			animation_ongoing = true;
			hero.direction = NORTH;
			break;
		case 's':
		case 'S':
			animation_ongoing = true;
			hero.direction = SOUTH;
			break;
		case 'a':
		case 'A':
			animation_ongoing = true;
			hero.direction = WEST;
			break;
		case 'd':
		case 'D':
			animation_ongoing = true;
			hero.direction = EAST;
			break;
		default:
			break;
	}
}

/* Namesta se pozicija iscrtavanja po prozoru,
 * proporcija display-a i vidljivost.
 * Namesta se jedinicna matrica da se ne bi primenjivala
 * afina transformacija vise puta pri 
 * visestrukoj izmeni velicine prozora.
 */
static void on_reshape(int width, int height)
{
	win_height = height;
	win_width = width;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)width / height, 1, 100);	
}

/* Onde gde se realizuje animacija
 */
static void on_timer(int value)
{
	if(value != TIMER_ID){
		return;
	}
	if(!animation_ongoing){
		glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
		return;
	}
	switch(hero.direction){
		case NORTH:
			hero.position_y += hero.velocity;
			break;
		case EAST:
			hero.position_x += hero.velocity;
			break;
		case SOUTH:
			hero.position_y -= hero.velocity;
			break;
		case WEST:
			hero.position_x -= hero.velocity;
			break;
		default:
			break;
	}

	if(collision_detection()){
		hero.direction = PASSIVE;
	}

	glutPostRedisplay();
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(500,500);
	glutCreateWindow("flosXONIX");
	glutKeyboardFunc(on_keyboard);
	glutReshapeFunc(on_reshape);
	glutDisplayFunc(on_display);
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
	glClearColor(0.75, 0.65, 0.55, 0);
	glEnable(GL_DEPTH_TEST);
	game_info_initialization();
	glutMainLoop();
	return 0;
}
