#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/bitmap.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>


ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *image = NULL;
ALLEGRO_LOCKED_REGION *region = NULL;

extern long filter_image(unsigned char *output_buffer, unsigned char *input_buffer, int mask, int width, int height);

void init_display(int y, int x)
{
   if (display)
      al_destroy_display(display);

   display = al_create_display(y, x);

   if (!display)
   {
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      exit(1);
   }
}

void init()
{
   al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_RGB_888);
   if (!al_init())
   {
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize allegro!",
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      exit(1);
   }

   if (!al_init_image_addon())
   {
      al_show_native_message_box(display, "Error", "Error", "Failed to initialize al_init_image_addon!",
                                 NULL, ALLEGRO_MESSAGEBOX_ERROR);
      exit(1);
   }
}

void read_image_from_user()
{
   al_destroy_bitmap(image);
   image = NULL;
   char name[100];
   do
   {
      printf("Please give name of the image: ");
      scanf("%s", name);
      image = al_load_bitmap(name);
   } while (!image);
}

void end_program()
{
   al_destroy_display(display);
   al_destroy_bitmap(image);
   al_shutdown_image_addon();
}

int readKernelsize()
{
   int value;
   printf("Please give the size of the kernel (should be odd number greater or equal 3: \n");
   scanf("%d", &value);
   return value;
 
}

int main(int argc, char *argv[])
{
   init();

   char control[10];
   while (true)
   {
      read_image_from_user();
      int width = al_get_bitmap_width(image);
      int height = al_get_bitmap_height(image);
      init_display(width,height);
      int widthInBytes = width * 3;
      int kernel;
      unsigned char *input_buffer = (unsigned char *)malloc(height * widthInBytes);

      //data points to the leftmost pixel of the first row (row 0) of the locked region.
      // format with 24 bits = 3*8 jeden bajt na pixel
      region = al_lock_bitmap(image, ALLEGRO_PIXEL_FORMAT_RGB_888, ALLEGRO_LOCK_READWRITE);
      memcpy(input_buffer, ((unsigned char *)(region->data)) - (height-1) * widthInBytes , height * widthInBytes);
      al_unlock_bitmap(image);

      while (true)
      {
         kernel=readKernelsize();
         region = al_lock_bitmap(image, ALLEGRO_PIXEL_FORMAT_RGB_888, ALLEGRO_LOCK_READWRITE);

         //filter_image(((unsigned char *)(region->data)) - (height-1) * widthInBytes , input_buffer, kernel, width, height);
         filter_image((unsigned char *)(region->data) + widthInBytes-1, input_buffer + (height-1) * widthInBytes-1, kernel, width, height);
         al_unlock_bitmap(image);

         al_draw_bitmap(image, 0, 0, 0);
         al_flip_display();

         printf("Do you want to try another kernel?(y/n) ");
         scanf("%s", control);
         if (control[0] != 'y')
            break;
      }

      printf("Do you want to try another image?(y/n) ");
      scanf("%s", control);
      if (control[0] != 'y')
      {
         free(input_buffer);
         break;
      }
   }

   end_program();

   return 0;
}