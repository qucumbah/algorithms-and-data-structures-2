#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>

#include "huffman.h"

static GtkWindow* window;

static GtkWidget* inputChooser;
static GtkWidget* outputChooser;
static GtkWidget* compressButton;
static GtkWidget* decompressButton;

static char* inputFilePath = NULL;
static char* outputFilePath = NULL;

void showInputDialog(GtkWidget* caller, gpointer data) {
  GtkWidget* dialog = gtk_file_chooser_dialog_new("Open File",
    window,
    GTK_FILE_CHOOSER_ACTION_OPEN,
    "_Cancel",
    GTK_RESPONSE_CANCEL,
    "_Open",
    GTK_RESPONSE_ACCEPT,
    NULL
  );

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    inputFilePath = gtk_file_chooser_get_filename(chooser);
  }

  gtk_widget_destroy(dialog);
}

void showOutputDialog(GtkWidget* caller, gpointer data) {
  GtkWidget* dialog = gtk_file_chooser_dialog_new(
    "Save File",
    window,
    GTK_FILE_CHOOSER_ACTION_SAVE,
    "_Cancel",
    GTK_RESPONSE_CANCEL,
    "_Open",
    GTK_RESPONSE_ACCEPT,
    NULL
  );

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    outputFilePath = gtk_file_chooser_get_filename(chooser);
  }

  gtk_widget_destroy(dialog);
}

bool checkIfFilesAreChosen() {
  if (inputFilePath != NULL && outputFilePath != NULL) {
    return true;
  }

  GtkWidget* dialog = gtk_message_dialog_new(
    window,
    GTK_DIALOG_DESTROY_WITH_PARENT,
    GTK_MESSAGE_WARNING,
    GTK_BUTTONS_CLOSE,
    "You have to specify input and output file paths"
  );

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

  return false;
}

void compress(GtkWidget* caller, gpointer data) {
  if (!checkIfFilesAreChosen()) {
    return;
  }

  printf("%s\n", inputFilePath);
  printf("%s\n", outputFilePath);

  huffmanEncode(inputFilePath, outputFilePath);
}

void decompress(GtkWidget* caller, gpointer data) {
  if (!checkIfFilesAreChosen()) {
    return;
  }

  huffmanDecode(inputFilePath, outputFilePath);
}

int main(int argc, char **argv) {
  GtkWidget* grid;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  grid = gtk_grid_new();
  int padding = 20;
  gtk_grid_set_column_spacing(grid, padding);
  gtk_grid_set_row_spacing(grid, padding);
  gtk_widget_set_margin_top(grid, padding);
  gtk_widget_set_margin_left(grid, padding);
  gtk_widget_set_margin_bottom(grid, padding);
  gtk_widget_set_margin_right(grid, padding);
  gtk_container_add(GTK_CONTAINER(window), grid);

  inputChooser = gtk_button_new_with_label("Choose input file");
  g_signal_connect(inputChooser, "clicked", G_CALLBACK(showInputDialog), NULL);
  gtk_grid_attach(GTK_GRID(grid), inputChooser, 0, 0, 1, 1);

  outputChooser = gtk_button_new_with_label("Choose output file");
  g_signal_connect(outputChooser, "clicked", G_CALLBACK(showOutputDialog), NULL);
  gtk_grid_attach(GTK_GRID(grid), outputChooser, 1, 0, 1, 1);

  compressButton = gtk_button_new_with_label("Compress");
  g_signal_connect(compressButton, "clicked", G_CALLBACK(compress), NULL);
  gtk_grid_attach(GTK_GRID(grid), compressButton, 0, 1, 1, 1);

  decompressButton = gtk_button_new_with_label("Decompress");
  g_signal_connect(decompressButton, "clicked", G_CALLBACK(decompress), NULL);
  gtk_grid_attach(GTK_GRID(grid), decompressButton, 1, 1, 1, 1);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
