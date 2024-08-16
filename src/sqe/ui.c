#include <gtk/gtk.h>
#include <math.h>
#include "sqe/ui.h"
#include "gtk/gtkshortcut.h"

// UI должно выглядеть как-то так:
//
// [1_] x^2 + [4_] x + [3_] = 0
// '--'       '--'     '--'
//   a          b        c
// -------------------------------
//
//  D = b^2 - 4ac = 4^2 - 4 * 1 * 3 = 4      <--- d_calc
//
//  D > 0 => решения два.                    <--- d_info
//               ___             ___
//       -b +- \/ D      -4 +- \/ 4
//  x = ------------- = -------------        <--- x_calc
//           2a              2 * 1                Прячется при x = {}
//
//    = { -3, -1 }                           <--- x_res
//
// Вывода мат. формул я пока не сделал, так пока что всё
// выводится как sqrt(x) и a / b.

/// Различные подписи, см. рисунок
GtkLabel *d_calc, *d_info, *x_calc, *x_res;

/// Значения параметров
double a_val = 1, b_val = 4, c_val = 3;

/// g_string_printf стирает пред. значение,
/// gtk_label_set_text копирует значения, так что 
GString* buf = NULL;

/// Если |a| < ZERO_EPS, то считаем что a = 0.
const double ZERO_EPS = 0.000001;

/// Заменяем текст подписи на printf() чего-то
/// g_string_vaprintf почему-то нет, так что приходится делать макрос.
#define upd_label_fmt(label, fmt, ...) \
  do {\
    g_string_printf(buf, fmt __VA_OPT__(,) __VA_ARGS__);\
    gtk_label_set_markup(label, buf->str);\
  } while(0)


/// Решаем уравнение, попутно обновляя подписи.
void upd_solution() {

  // Не перерешиваем в случае неправильного ввода.
  if (isnan(a_val) || isnan(b_val) || isnan(c_val)) return;

  if (fabs(a_val) < ZERO_EPS) { // bx + c = 0
    if (fabs(b_val) < ZERO_EPS) { // c = 0
      if (fabs(c_val) < ZERO_EPS) { // 0 = 0
        upd_label_fmt(x_res, "x ∈ ℝ");
      } else { // c = 0, в то время как c != 0
        upd_label_fmt(x_res, "x = ∅");
      }
    } else {
      upd_label_fmt(x_res, "x = -<i>c</i>/<i>b</i> = - %lf / %lf = %lf", c_val, b_val, - c_val / b_val);
    }
    gtk_widget_set_visible(GTK_WIDGET(d_calc), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(d_info), FALSE);
    gtk_widget_set_visible(GTK_WIDGET(x_calc), FALSE);
  } else {
    // Уравнение всё же квадратное.
    gtk_widget_set_visible(GTK_WIDGET(d_calc), TRUE);
    gtk_widget_set_visible(GTK_WIDGET(d_info), TRUE);

    double D = b_val * b_val - 4 * a_val * c_val;

    // Рассчёт D
    upd_label_fmt(d_calc, "<i>D</i> = <i>b</i><sup>2</sup> - 4<i>ac</i>"
                        " = %lf<sup>2</sup> - 4 * %lf * %lf = %lf",
                b_val, a_val, c_val, D);

    if (fabs(D) < ZERO_EPS) { // D = 0
      upd_label_fmt(d_info, "<i>D</i> = 0 ⇒ решение одно");
      double res = -b_val / 2 / a_val;
      gtk_widget_set_visible(GTK_WIDGET(x_calc), TRUE);
      upd_label_fmt(x_calc, "<i>x</i> = -<i>b</i> / 2<i>a</i> = -%lf / (2 * %lf)", b_val, a_val);
      upd_label_fmt(x_res, " = %lf", res);
    } else if (D < 0) { // D < 0, решений нет.
      upd_label_fmt(d_info, "<i>D</i> &lt; 0 ⇒ решений нет");
      gtk_widget_set_visible(GTK_WIDGET(x_calc), FALSE);
      upd_label_fmt(x_res, "<i>x</i> = ∅");
    } else if (D > 0) { // D > 0, два решения
      upd_label_fmt(d_info, "<i>D</i> > 0 ⇒ решения два");
      double res1 = (-b_val - sqrt(D)) / 2 / a_val,
             res2 = (-b_val + sqrt(D)) / 2 / a_val;
      gtk_widget_set_visible(GTK_WIDGET(x_calc), TRUE);
      upd_label_fmt(x_calc, "<i>x</i> = (-<i>b</i> ± sqrt(<i>D</i>)) / 2<i>a</i>"
                            " = (-%lf ± sqrt(%lf)) / (2 * %lf)", b_val, D, a_val);
      upd_label_fmt(x_res, " = {%lf; %lf}", res1, res2);
    }
  }
}

/// Обработчик изменения значения поля ввода a, b или с.
void _input_changed(GtkWidget* entry, gpointer var) {
  // Получаем введёное значение
  GtkEntryBuffer* ent_buf = gtk_entry_get_buffer(GTK_ENTRY(entry));
  const char* text = gtk_entry_buffer_get_text(ent_buf);
  char* end;
  double res;

  if (*text == 0) // ничего не введено
    goto bad_value;
   
  res = strtod(text, &end);

  if (*end) // лишние символы, ...
    goto bad_value;

  if (isnan(res) || res == HUGE_VAL) // Переполнилось / пользователь ввёл NAN
    goto bad_value;

  *((double*) var) = res;
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, NULL);
  upd_solution();

  return;

bad_value:
  // Показываем пользователю иконку.
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(entry), GTK_ENTRY_ICON_SECONDARY, "dialog-warning-symbolic");
}

/// Делаем переданный виджет выровненным по левой границе контейнера
GtkWidget* left_aligned(GtkWidget* wgt) {
  gtk_widget_set_halign(wgt, GTK_ALIGN_START);
  return wgt;
}

/// Создаём новую подпись наподобие gtk_label_new,
/// только с форматированным текстом
GtkWidget* markup_label(const char* text) {
  GtkWidget* lbl = gtk_label_new("");
  gtk_label_set_markup(GTK_LABEL(lbl), text);
  return lbl;
}

/// Устанавливаем значение поля ввода.
void set_entry_text(GtkEntry* entry, const char* text) {
  gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), text, strlen(text));
}

void sqe_build_ui(GtkWindow *window) {
  // Создаём буффер для upd_label_fmt()
  buf = g_string_new("");

  // Главный вертикальный flexbox (в Gtk это просто Box).
  GtkBox* main_layout = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));
  gtk_widget_set_halign(GTK_WIDGET(main_layout), GTK_ALIGN_FILL);
  gtk_widget_set_valign(GTK_WIDGET(main_layout), GTK_ALIGN_START);
  // Отступы. Почему разработчики не сделали set_margin()?
  gtk_widget_set_margin_start(GTK_WIDGET(main_layout), 20);
  gtk_widget_set_margin_end(GTK_WIDGET(main_layout), 20);
  gtk_widget_set_margin_top(GTK_WIDGET(main_layout), 20);
  gtk_widget_set_margin_bottom(GTK_WIDGET(main_layout), 20);

  // Заголовок
  gtk_box_append(main_layout, left_aligned(markup_label("<span size='x-large'>Решатель квадратных уравнений</span>")));

  // Верхняя часть, с полями ввода.
  GtkEntry *a = GTK_ENTRY(gtk_entry_new()),
           *b = GTK_ENTRY(gtk_entry_new()),
           *c = GTK_ENTRY(gtk_entry_new());

  set_entry_text(a, "1");
  set_entry_text(b, "4");
  set_entry_text(c, "3");

  gtk_entry_set_input_purpose(a, GTK_INPUT_PURPOSE_NUMBER);
  gtk_entry_set_input_purpose(b, GTK_INPUT_PURPOSE_NUMBER);
  gtk_entry_set_input_purpose(c, GTK_INPUT_PURPOSE_NUMBER);

  g_signal_connect(a, "changed", G_CALLBACK(_input_changed), &a_val); 
  g_signal_connect(b, "changed", G_CALLBACK(_input_changed), &b_val); 
  g_signal_connect(c, "changed", G_CALLBACK(_input_changed), &c_val); 

  GtkBox* top_layout = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
  gtk_widget_set_halign(GTK_WIDGET(top_layout), GTK_ALIGN_START);
  gtk_box_append(top_layout, GTK_WIDGET(a));
  gtk_box_append(top_layout, markup_label("<i>x</i><sup>2</sup> + "));
  gtk_box_append(top_layout, GTK_WIDGET(b));
  gtk_box_append(top_layout, markup_label("<i>x</i> + "));
  gtk_box_append(top_layout, GTK_WIDGET(c));
  gtk_box_append(top_layout, markup_label(" = 0"));
  gtk_box_append(main_layout, GTK_WIDGET(top_layout));

  // Разделитель
  gtk_box_append(main_layout, gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

  // Строки текста
  gtk_box_append(main_layout, GTK_WIDGET(d_calc = GTK_LABEL(left_aligned(gtk_label_new("")))));
  gtk_box_append(main_layout, GTK_WIDGET(d_info = GTK_LABEL(left_aligned(gtk_label_new("")))));
  gtk_box_append(main_layout, GTK_WIDGET(x_calc = GTK_LABEL(left_aligned(gtk_label_new("")))));
  gtk_box_append(main_layout, GTK_WIDGET(x_res = GTK_LABEL(left_aligned(gtk_label_new("")))));

  gtk_window_set_child(window, GTK_WIDGET(main_layout));

  upd_solution();
}
