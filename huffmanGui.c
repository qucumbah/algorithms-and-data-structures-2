#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <gtk/gtk.h>

#include "huffman.h"
#include "util.h"

static GtkWindow* window;

static GtkEntryBuffer* inputBuffer;
static GtkEntryBuffer* outputBuffer;
static GtkTextBuffer* resultBuffer;

static GtkWidget* inputField;
static GtkWidget* outputField;
static GtkWidget* inputChooser;
static GtkWidget* outputChooser;
static GtkWidget* compressButton;
static GtkWidget* decompressButton;
static GtkWidget* resultTextArea;

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
    gchar* inputFilePath = gtk_file_chooser_get_filename(chooser);
    gtk_entry_buffer_set_text(inputBuffer, inputFilePath, -1);
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
    gchar* outputFilePath = gtk_file_chooser_get_filename(chooser);
    gtk_entry_buffer_set_text(outputBuffer, outputFilePath, -1);
  }

  gtk_widget_destroy(dialog);
}

bool checkIfFilesAreChosen() {
  if (gtk_entry_buffer_get_bytes(inputBuffer) != 0 && gtk_entry_buffer_get_bytes(outputBuffer)) {
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

  char* inputFilePath = gtk_entry_buffer_get_text(inputBuffer);
  char* outputFilePath = gtk_entry_buffer_get_text(outputBuffer);

  char* buf = malloc(2048);

  struct timeval time;

  gettimeofday(&time, NULL);
  long start = time.tv_usec;
  
  huffmanEncode(inputFilePath, outputFilePath);

  gettimeofday(&time, NULL);
  long end = time.tv_usec;

  sprintf(
    buf,
    "Compressed %s to %s\nSize before: %d bytes\nSize after: %d bytes\nTime taken: %.1lfms",
    inputFilePath,
    outputFilePath,
    fileSize(inputFilePath),
    fileSize(outputFilePath),
    (double)(end - start) / 1000
  );

  gtk_text_buffer_set_text(resultBuffer, buf, -1);
}

void decompress(GtkWidget* caller, gpointer data) {
  char* inputFilePath = gtk_entry_buffer_get_text(inputBuffer);
  char* outputFilePath = gtk_entry_buffer_get_text(outputBuffer);

  char* buf = malloc(2048);

  struct timeval time;

  gettimeofday(&time, NULL);
  long start = time.tv_usec;
  
  huffmanDecode(inputFilePath, outputFilePath);

  gettimeofday(&time, NULL);
  long end = time.tv_usec;

  sprintf(
    buf,
    "Decompressed %s to %s\nSize before: %d bytes\nSize after: %d bytes\nTime taken: %.1lfms",
    inputFilePath,
    outputFilePath,
    fileSize(inputFilePath),
    fileSize(outputFilePath),
    (double)(end - start) / 1000
  );

  gtk_text_buffer_set_text(resultBuffer, buf, -1);
}

int main(int argc, char **argv) {
  GtkWidget* grid;
  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(window, "Huffman algorithm");
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

  inputBuffer = gtk_entry_buffer_new(NULL, -1);
  outputBuffer = gtk_entry_buffer_new(NULL, -1);
  resultBuffer = gtk_text_buffer_new(NULL);

  gtk_entry_buffer_set_text(inputBuffer, "/home/dshmelev/Code/ast2/book.txt", -1);
  gtk_entry_buffer_set_text(outputBuffer, "/home/dshmelev/Code/ast2/1", -1);

  inputField = gtk_entry_new_with_buffer(inputBuffer);
  // gtk_widget_set_size_request(inputField, 200, 30);
  gtk_grid_attach(GTK_GRID(grid), inputField, 0, 0, 1, 1);

  outputField = gtk_entry_new_with_buffer(outputBuffer);
  // gtk_widget_set_size_request(outputField, 200, 30);
  gtk_grid_attach(GTK_GRID(grid), outputField, 0, 1, 1, 1);

  inputChooser = gtk_button_new_with_label("Choose input file");
  g_signal_connect(inputChooser, "clicked", G_CALLBACK(showInputDialog), NULL);
  gtk_grid_attach(GTK_GRID(grid), inputChooser, 1, 0, 1, 1);

  outputChooser = gtk_button_new_with_label("Choose output file");
  g_signal_connect(outputChooser, "clicked", G_CALLBACK(showOutputDialog), NULL);
  gtk_grid_attach(GTK_GRID(grid), outputChooser, 1, 1, 1, 1);

  compressButton = gtk_button_new_with_label("Compress");
  g_signal_connect(compressButton, "clicked", G_CALLBACK(compress), NULL);
  gtk_grid_attach(GTK_GRID(grid), compressButton, 0, 2, 1, 1);

  decompressButton = gtk_button_new_with_label("Decompress");
  g_signal_connect(decompressButton, "clicked", G_CALLBACK(decompress), NULL);
  gtk_grid_attach(GTK_GRID(grid), decompressButton, 1, 2, 1, 1);

  resultTextArea = gtk_text_view_new_with_buffer(resultBuffer);
  gtk_text_view_set_top_margin(resultTextArea, padding);
  gtk_text_view_set_left_margin(resultTextArea, padding);
  gtk_text_view_set_bottom_margin(resultTextArea, padding);
  gtk_text_view_set_right_margin(resultTextArea, padding);
  gtk_grid_attach(GTK_GRID(grid), resultTextArea, 0, 3, 2, 1);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
