#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Definitions.h"

void add_point(My_Polygon *pg, Window *wd, int x, int y) {
	pg->point[pg->n_points][0] = 2.0f * ((float)x) / wd->width - 1.0f;
	pg->point[pg->n_points][1] = 2.0f * ((float)wd->height - y) / wd->height - 1.0f;
	pg->n_points++; 
}

void close_line_segments(My_Polygon *pg) {
	pg->point[pg->n_points][0] = pg->point[0][0];
	pg->point[pg->n_points][1] = pg->point[0][1];
	pg->n_points++;
}

void draw_lines_by_points(My_Polygon* pg, float zoom_scale, int move_mode) {				//현재 확대 비율과 이동 모드인지에 따른 polygon 그리기
	if (move_mode) {

			glColor3f(POINT_COLOR);
			for (int i = 0; i < pg->n_points; i++) {
				glBegin(GL_POINTS);
				glVertex2f(pg->center_x + (pg->point[i][0] - pg->center_x) * zoom_scale, pg->center_y + (pg->point[i][1] - pg->center_y) * zoom_scale);		//zoom_scale을 곱해줌으로써 점을 그릴 때 확대 비율 적용
				glEnd();
			}
			glColor3f(LINE_MOVE_COLOR);									//이동 모드라면 빨간색으로 선분 그리기
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < pg->n_points; i++)
				glVertex2f(pg->center_x + (pg->point[i][0] - pg->center_x) * zoom_scale, pg->center_y + (pg->point[i][1] - pg->center_y) * zoom_scale);		//zoom_scale을 곱해줌으로써 선분을 그릴 때 확대 비율 적용
		
		glEnd();
	}
	else {
		if (pg->center_x == 0.0f && pg->center_y == 0.0f) {				//아직 polygon을 완성하지 않았다면
			glColor3f(POINT_COLOR);
			for (int i = 0; i < pg->n_points; i++) {
				glBegin(GL_POINTS);
				glVertex2f(pg->point[i][0], pg->point[i][1]);
				glEnd();
			}
			glColor3f(LINE_COLOR);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < pg->n_points; i++)
				glVertex2f(pg->point[i][0], pg->point[i][1]);
		}
		else {
			glColor3f(POINT_COLOR);
			for (int i = 0; i < pg->n_points; i++) {
				glBegin(GL_POINTS);
				glVertex2f(pg->center_x + (pg->point[i][0] - pg->center_x) * zoom_scale, pg->center_y + (pg->point[i][1] - pg->center_y) * zoom_scale);
				glEnd();
			}
			glColor3f(LINE_COLOR);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < pg->n_points; i++)
				glVertex2f(pg->center_x + (pg->point[i][0] - pg->center_x) * zoom_scale, pg->center_y + (pg->point[i][1] - pg->center_y) * zoom_scale);
		}
		glEnd();
	}

}

void update_center_of_gravity(My_Polygon* pg) {
	pg->center_x = pg->center_y = 0.0f;
	if (pg->n_points == 0) return;
	for (int i = 0; i < pg->n_points; i++) {
		pg->center_x += pg->point[i][0], pg->center_y += pg->point[i][1];
	}
	pg->center_x /= (float)pg->n_points, pg->center_y /= (float)pg->n_points;
}

void move_points(My_Polygon* pg, float del_x, float del_y) {
	for (int i = 0; i < pg->n_points; i++) {
		pg->point[i][0] += del_x, pg->point[i][1] += del_y;
	}
}

void rotate_points_around_center_of_grivity(My_Polygon* pg) {
	for (int i = 0; i < pg->n_points; i++) {
		float x, y;
		x = COS_5_DEGREES * (pg->point[i][0] - pg->center_x)
			- SIN_5_DEGREES * (pg->point[i][1] - pg->center_y) + pg->center_x;
		y = SIN_5_DEGREES * (pg->point[i][0] - pg->center_x)
			+ COS_5_DEGREES * (pg->point[i][1] - pg->center_y) + pg->center_y;
		pg->point[i][0] = x, pg->point[i][1] = y;
	}
}