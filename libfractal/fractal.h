#ifndef _FRACTAL_H
#define _FRACTAL_H

struct fractal {
  char *name;
  int width;
  int height;
  double a;
  double b;
  int *value;// a voir si c'est pas plutot des int.
};

/*
 * fractal_new: alloue une nouvelle structure fractal
 *
 * @name: nom de la fractale
 * @width: largeur de l'image finale
 * @height: hauteur de l'image finale
 * @a: partie réelle des coordonnées de la fractale
 * @b: partie imaginaire des coordonnées de la fractale
 * @return: un pointeur vers une struct fractal, ou NULL si erreur
 */
struct fractal *fractal_new(const char *name, int width, int height, double a, double b){
  struct fractal *new;
  new=(struct fractal *) malloc(sizeof(struct fractal));
  if(new==NULL){
    return NULL;
  }
  new->name=name;
  new->width=width;
  new->height=height;
  new->a=a;
  new->b=b;
  double value_test[height][width];
  new->value=(double *)value_test;
  return new;
}

/*
 * fractal_free: libère la mémoire utilisée par une struct fractal
 *
 * @f: fractale à libérer
 */
void fractal_free(struct fractal *f){
free(f);
}

/*
 * fractal_get_name: retourne le nom de la fractale
 *
 * @f: fractale
 * @return: nom de la fractale
 */
const char *fractal_get_name(const struct fractal *f);{
  return (f->name);
}

/*
 * fractal_get_value: retourne la valeur correspondant à un pixel de l'image
 *
 * @f: fractale
 * @x: abscisse
 * @y: ordonnée
 * @return: valeur du pixel (x,y) de l'image de la fractale
 */
int fractal_get_value(const struct fractal *f, int x, int y){
  int r=f->width;
  return *(f->value+(y-1)*r+x);
}

/*
 * fractal_set_value: défini la valeur correspondant à un pixel de l'image
 *
 * @f: fractale
 * @x: abscisse
 * @y: ordonnée
 * @val: valeur
 */
void fractal_set_value(struct fractal *f, int x, int y, int val){
  int r=f->width;
  *(f->value+(y-1)*r+x)=val;
}

/*
 * fractal_get_width: retourne la largeur de l'image de la fractale
 *
 * @f: fractale
 * @return: largeur
 */
int fractal_get_width(const struct fractal *f){
  return (f->width);
}

/*
 * fractal_get_height: retourne la hauteur de l'image de la fractale
 *
 * @f: fractale
 * @return: hauteur
 */
int fractal_get_height(const struct fractal *f){
  return(f->height);
}

/*
 * fractal_get_a: retourne la partie réelle des coordonnées de la fractale
 *
 * @f: fractale
 * @return: partie réelle
 */
double fractal_get_a(const struct fractal *f){
  return (f->a);
}

/*
 * fractal_get_b: retourne la partie imaginaire des coordonnées de la fractale
 *
 * @f: fractale
 * @return: partie imaginaire
 */
double fractal_get_b(const struct fractal *f){
  return (f->b);
}

/*
 * fractal_compute_value
 *
 * Applique la valeur de récurrence sur la fractale, aux coordonnées correspondantes
 * au pixel (x,y) de l'image finale, et appelle fractal_set_value() pour
 * enregistrer le nombre d'itérations effectuées.
 *
 * @f: fractale
 * @x: abscisse
 * @y: ordonnée
 * @return: nombre d'itérations
 */
int fractal_compute_value(struct fractal *f, int x, int y){
      double zx, zy;
      double a, b;
      int val;
      int w, h;

      a = fractal_get_a(f);
      b = fractal_get_b(f);

      w = fractal_get_width(f);
      h = fractal_get_height(f);

      zx = ((double)x / (double)w) * 3.0 - 1.5;
      zy = ((double)y / (double)h) * 3.0 - 1.5;

      val = iter_julia(zx, zy, a, b, 0);
      fractal_set_value(f, x, y, val);

      return val;
}

/*
 * write_bitmap_sdl
 *
 * Transforme une fractale en un fichier bitmap (BMP)
 *
 * @f: fractale à transformer
 * @fname: nom du fichier de sortie
 * @return: 0 si pas d'erreurs, -1 sinon
 */
int write_bitmap_sdl(const struct fractal *f, const char *fname){
  SDL_Surface *back;
  SDL_Rect pix;
  int w, h, i, j, col;
  uint32_t pcol, rcol, gcol, bcol;
  w = fractal_get_width(f);
  h = fractal_get_height(f);

  back = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);
  if (!back)
      return -1;

  for (i = 0; i < w; i++) {
      for (j = 0; j < h; j++) {
          col = itoc(fractal_get_value(f, i, j));
          rcol = col & 0xff;
          gcol = (col >> 8) & 0xff;
          bcol = (col >> 16) & 0xff;
          pcol = SDL_MapRGB(back->format, rcol, gcol, bcol);

          pix.w = pix.h = 1;
          pix.x = i;
          pix.y = j;
          SDL_FillRect(back, &pix, pcol);
      }
  }

  if (SDL_SaveBMP(back, fname) < 0)
      return -1;

  SDL_FreeSurface(back);

  return 0;
}



static int iter_julia(double zx, double zy, double a, double b, int it)
{
    /* prevent infinite loop */
    if (it > MAX_ITER)
        return 0;

    /* prevent leaving Julia set
     * if distance to origin >= 2
     */
    if (zx*zx + zy*zy >= 4.0)
        return it;

    /* compute next iteration
     * f(z) = z^2 + c
     * z = x + yi
     * c = a + bi
     */
    return iter_julia(zx*zx - zy*zy + a, 2*zx*zy + b, a, b, it+1);
}

#endif
