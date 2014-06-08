/*
 * display.cpp
 */

#include "display.h"
#include "movement.h"
#include "global.h"
#include <cstdio>
#include <iostream>


//Could place these inside of the object files and refer to them from there

	//weapon
  glm::vec3 WeaPos(0.0, -0.1, 0.0);
  int WeaTimer = 50000;



void display (void) {
  movement();
  for (int i=0; i<NUMENTITIES; i++) {
    entitiesArray[i]->update(entitiesArray, NUMENTITIES);
  }


  glm::vec3 axis_y(0, 1, 0);
  glm::mat4 anim;
  glm::mat4 view;
  glm::mat4 model;
  glm::mat4 projection;
  glm::mat4 current_mvp;

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
	//1st screen top
  glViewport(0, SCREENHEIGHT/2+1, SCREENWIDTH, SCREENHEIGHT/2);
  view = glm::lookAt(player1->viewSrc, player1->viewDest, player1->viewUp);

  projection = glm::perspective(45.0f, 1.0f*SCREENWIDTH/SCREENHEIGHT, 0.1f, 100.0f);

  //PLAYER1
  anim = glm::rotate(glm::mat4(1.0f),degToRad(player1->angle), axis_y);
  model = glm::translate(glm::mat4(1.0f), player1->pos);
  current_mvp = projection * view * model * anim;

 drawObject(player1, current_mvp, model, view);

 //PLAYER2
 anim = glm::rotate(glm::mat4(1.0f), degToRad(player2->angle), axis_y);
 model = glm::translate(glm::mat4(1.0f), player2->pos);
 current_mvp = projection * view * model * anim;

 drawObject(player2, current_mvp, model, view);

  //Weapon
  if(WeaTimer < 5000){
	 anim = glm::rotate(glm::mat4(1.0f), degToRad(weapon->angle), axis_y);
	 model = glm::translate(glm::mat4(1.0f), WeaPos);
	 current_mvp = projection * view * model * anim;

	 drawObject(weapon, current_mvp, model, view);
  }


//---------------------------


	//2ed screen, bottom
  glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT/2);
  view = glm::lookAt(player2->viewSrc, player2->viewDest, player2->viewUp);

  //PLAYER1
  anim = glm::rotate(glm::mat4(1.0f),degToRad(player1->angle), axis_y);
  model = glm::translate(glm::mat4(1.0f), player1->pos);
  current_mvp = projection * view * model * anim;

 drawObject(player1, current_mvp, model, view);

 //PLAYER2
 anim = glm::rotate(glm::mat4(1.0f),degToRad(player2->angle) , axis_y);
 model = glm::translate(glm::mat4(1.0f), player2->pos);
 current_mvp = projection * view * model * anim;

 drawObject(player2, current_mvp, model, view);

  if(WeaTimer < 5000){
	 anim = glm::rotate(glm::mat4(1.0f), degToRad(weapon->angle), axis_y);
	 model = glm::translate(glm::mat4(1.0f), WeaPos);
	 current_mvp = projection * view * model * anim;

	 drawObject(weapon, current_mvp, model, view);
  }


 glutSwapBuffers();
}

void drawObject(Entity * my_entity,  glm::mat4 current_mvp, glm::mat4 model, glm::mat4 view){
  struct model_info my_model = my_entity->my_model;
  struct shader_program_info my_program = my_entity->my_program;
  GLMmodel* model_ptr = my_entity->model_ptr;
  
  glUseProgram(my_program.program);

  //Now no matter what group we are going to draw from these vertices..
  glEnableVertexAttribArray(my_program.attribute_coord3d);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glBindBuffer(GL_ARRAY_BUFFER, my_model.vbo_model_vertices);
  glVertexAttribPointer(
    my_program.attribute_coord3d, // attribute
    3,                 // number of elements per vertex, here (x,y,z)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is
    0,                 // no extra data between each position
    0                  // offset of first element
  );
  //Now no matter what group we are going to draw from these normals..
  glEnableVertexAttribArray(my_program.attribute_v_normal);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glBindBuffer(GL_ARRAY_BUFFER, my_model.vbo_model_normals);
  glVertexAttribPointer(
    my_program.attribute_v_normal, // attribute
    3,                 // number of elements per vertex, here (x,y,z)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is
    0,                 // no extra data between each position
    0                  // offset of first element
  );
  //and no matter what group we are going to draw from these texcoords
  glEnableVertexAttribArray(my_program.attribute_texcoord);
  // Describe our texcooords array to OpenGL (it can't guess its format automatically)
  glBindBuffer(GL_ARRAY_BUFFER, my_model.vbo_model_texcoords);
  glVertexAttribPointer(
    my_program.attribute_texcoord, // attribute
    2,                 // number of elements per vertex, here (s,t)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is
    0,                 // no extra data between each position
    0                  // offset of first element
  );

    glUniformMatrix4fv(my_program.uniform_mvp, 1, GL_FALSE, glm::value_ptr(current_mvp));
    glUniformMatrix4fv(my_program.uniform_m, 1, GL_FALSE, glm::value_ptr(model));

 	glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(model)));
    glUniformMatrix3fv(my_program.uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
	glm::mat4 v_inv = glm::inverse(view);
	glUniformMatrix4fv(my_program.uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

   glActiveTexture(GL_TEXTURE0);
//loop through groups
    GLMgroup* group=model_ptr->groups;
    int current_group=0;
    while (group)
      { 
     if ((group->numtriangles) ==0 ) //have nothing to draw
          {
        group=group->next;
        current_group++;
          break;
          }
  //Push each element in buffer_vertices to the vertex shader
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_model.ibo_model_elements[current_group]);
  //how many elements in what we want to draw?? Sure, we could have kept this as state rather than enquiring now.....
  int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  
 //   glUniform3fv(my_program.uniform_Kd,1,(const GLfloat *)  &(model_ptr->materials[group->material].diffuse));

  glBindTexture(GL_TEXTURE_2D, my_model.model_texture_ids[current_group]);
   glUniform1i(my_program.uniform_texture, 0);//0 is GL_TEXTURE

   glDrawElements(GL_TRIANGLES, size/sizeof(GLuint), GL_UNSIGNED_INT, 0);
        group=group->next;
        current_group++;
   }//end looping through groups
}
