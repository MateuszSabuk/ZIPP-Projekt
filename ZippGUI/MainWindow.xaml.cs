﻿using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;

using CLI;

using System.Diagnostics;
using System.Text.RegularExpressions;
using static System.Runtime.InteropServices.JavaScript.JSType;
using System.Reflection.PortableExecutable;
using System.Collections;
using System.Data;
using System;
using System.Collections.Generic;
using System.Windows.Threading;
using System.IO;
using Microsoft.Win32;
using System.Windows.Diagnostics;

namespace ZippGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private SolverManager manager;
        private int[] machines = { };
        private int[,] taskTimes = { };
        private Thread? runThread;
        private Stack<string> visualisationTaskNames = new Stack<string>();

        // Display timer
        DateTime runStartTime = DateTime.Now;
        DispatcherTimer runTimer;
        bool runTimerTriggered = false;

        // Populate main window values
        public MainWindow()
        {
            manager = new SolverManager();
            InitializeComponent();
            runTimer = new DispatcherTimer(new TimeSpan(0, 0, 0, 0, 50), DispatcherPriority.Background, callback: runTimer_Tick, Dispatcher.CurrentDispatcher);
            runTimer.Stop();
            // Set default values for the inputs
            numOfStages.Text = "4";
            numOfTasks.Text = "10";
            maxNumOfMachinesInStage.Text = "5";
            maxTaskTime.Text = "20";
        }

        // /////////////// Generator section ///////////////

        /// <summary>
        /// Onclick for GenerateButton. <br/>
        /// <list type="number" >
        /// <item>Parses generate inputs and runs generator with the params.</item>
        /// <item>Populates machines and taskTimes arrays.</item>
        /// <item>Runs fillInstanceDataGrids and createGanttVisualisationStages</item>
        /// </list>
        /// </summary>
        private void GenerateButtonClick(object sender, RoutedEventArgs e)
        {
            // No new generation during execution of another algorithm
            if (runThread != null && runThread.IsAlive) return;
            try
            {
                int s = int.Parse(numOfStages.Text);
                int n = int.Parse(numOfTasks.Text);
                int maxMIS = int.Parse(maxNumOfMachinesInStage.Text);
                int maxTT = int.Parse(maxTaskTime.Text);
                Tuple<int[], int[,]> data = manager.generate(s, n, maxMIS, maxTT);
                machines = data.Item1;
                taskTimes = data.Item2;
                fillInstanceDataGrids();
                createGanttVisualisationStages();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Generate error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        /// <summary>
        /// Fills Instance tables using data from machines and taksTimes arrays.<br/>
        /// </summary>
        private void fillInstanceDataGrids()
        {
            // Convert machines (1D array) to string
            DataTable machinesTable = new DataTable();

            // Add columns
            for (int col = 0; col < taskTimes.GetLength(1); col++)
            {
                machinesTable.Columns.Add($"Stage {col + 1}");
            }
            DataRow machineDataRow = machinesTable.NewRow();
            for (int col = 0; col < taskTimes.GetLength(1); col++)
            {
                machineDataRow[col] = machines[col];
            }
            machinesTable.Rows.Add(machineDataRow);
            MachinesDataGrid.ItemsSource = machinesTable.DefaultView;

            // Convert taskTimes (2D array) to string
            DataTable tasksTable = new DataTable();

            tasksTable.Columns.Add($"ID");
            // Add columns
            for (int col = 0; col < taskTimes.GetLength(1); col++)
            {
                tasksTable.Columns.Add($"Stage {col + 1}");
            }

            // Add rows
            for (int row = 0; row < taskTimes.GetLength(0); row++)
            {
                DataRow dataRow = tasksTable.NewRow();
                dataRow[0] = row;
                for (int col = 0; col < taskTimes.GetLength(1); col++)
                {
                    dataRow[col+1] = taskTimes[row, col];
                }
                tasksTable.Rows.Add(dataRow);
            }
            TasksDataGrid.ItemsSource = tasksTable.DefaultView;
            TasksDataGrid.Columns[0].IsReadOnly = true;
        }

        /// <summary>
        /// Creates the machines on the gantt visualisation (placeholders for the tasks)
        /// </summary>
        private void createGanttVisualisationStages()
        {
            while (visualisationTaskNames.Count > 0)
            {
                string name = visualisationTaskNames.Pop();
                if (FindName(name) is FrameworkElement)
                UnregisterName(name);
            }
            VisualisationStageStackPanel.Children.Clear();
            VisualisationStackPanel.Children.Clear();
            VisualisationStackPanel.MinWidth = VisualisationStackPanel.ViewportWidth;
            for (int stageIndex = 0; stageIndex < machines.Length; stageIndex++)
            {
                Grid stageGroup = new Grid
                {
                    Height = 17 * machines[stageIndex] -2,
                    MinWidth = 40,
                    Background = Brushes.Gray,
                    Margin = new Thickness(0, 7, 0, 5)
                };
                stageGroup.Children.Add(new TextBlock {
                    Text = (stageIndex + 1).ToString(),
                    Margin = new Thickness(5,0,5,0),
                    VerticalAlignment = VerticalAlignment.Center,
                    HorizontalAlignment = HorizontalAlignment.Center,
                    FontWeight = FontWeights.Bold,
                    Foreground = Brushes.White
                });

                VisualisationStageStackPanel.Children.Add(stageGroup);

                StackPanel stageRow = new StackPanel { Orientation = Orientation.Vertical, Margin = new Thickness(0, 5, 0, 5) };

                for (int machineIndex = 0; machineIndex < machines[stageIndex]; machineIndex++)
                {
                    visualisationTaskNames.Push("machineRow_stage" + stageIndex + "_machine" + machineIndex);
                    Canvas machineRow = new Canvas
                    {
                        Name = visualisationTaskNames.Peek(),
                        Height = 15,
                        Background = Brushes.Gray,
                        Margin = new Thickness(5, 2, 0, 0)
                    };
                    RegisterName(machineRow.Name, machineRow);
                    stageRow.Children.Add(machineRow);
                }
                VisualisationStackPanel.Children.Add(stageRow);
            }
        }

        // /////////////// Algorithm choice and parameters selection section ///////////////

        /// <summary>
        /// Runs when the algorithm selection ComboBox is loaded to the GUI<br/>
        /// Gets list of names of algorithms from the backend and puts them into the ComboBox
        /// </summary>
        private void AlgorithmChoice_Loaded(object sender, EventArgs e)
        {
            string[] algorithmNames = manager.getAlgorithmNames();

            AlgorithmChoice.ItemsSource = algorithmNames;
            if (algorithmNames.Length > 0)
            {
                AlgorithmChoice.SelectedIndex = 0;
            }
        }

        /// <summary>
        /// Called when algorithm selection ComboBox value changes<br/>
        /// Calls PopulateAlgorithmParams for the chosen algorithm
        /// </summary>
        private void AlgorithmChoice_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (runThread != null && runThread.IsAlive) return;
            PopulateAlgorithmParams(AlgorithmChoice.SelectedIndex);
        }


        /// <summary>
        /// Gets the parameters from the chosen algorithm and displays them
        /// </summary>
        private void PopulateAlgorithmParams(int algId)
        {
            // Get the algorithm parameters from the getAlgorithmParams method
            Dictionary<string, int> parameters = manager.getAlgorithmParams(algId);

            // Clear existing children in the AlgorithmParamsPanel
            AlgorithmParamsPanel.Children.Clear();

            foreach (var param in parameters)
            {
                // Create a StackPanel to hold the label and textbox
                StackPanel paramPanel = new StackPanel { Orientation = Orientation.Horizontal, Margin = new Thickness(0, 5, 0, 5) };

                // Create the label for the parameter
                Label label = new Label
                {
                    Content = param.Key,
                    Width = 150,
                    VerticalAlignment = VerticalAlignment.Center
                };

                // Create the TextBox for the parameter value
                TextBox textBox = new TextBox
                {
                    Width = 100,
                    Text = param.Value.ToString(),
                    Background = System.Windows.Media.Brushes.White,
                    BorderBrush = System.Windows.Media.Brushes.Gray,
                    Padding = new Thickness(5),
                    Margin = new Thickness(10, 0, 0, 0)
                };
                textBox.PreviewTextInput += new TextCompositionEventHandler(NumberValidationTextBoxChange);
                textBox.LostFocus += new RoutedEventHandler(TextBox_LostFocus);

                // Add the label and textbox to the paramPanel
                paramPanel.Children.Add(label);
                paramPanel.Children.Add(textBox);

                // Add the paramPanel to the AlgorithmParamsPanel
                AlgorithmParamsPanel.Children.Add(paramPanel);
            }
        }

        /// <summary>
        /// Called on click of SetParameters button<br/>
        /// Sends the parameters of the chosen algorithm to the backend
        /// </summary>
        private void SetParametersButtonClick(object sender, RoutedEventArgs e)
        {
            try
            {
                if (runThread != null && runThread.IsAlive) return;
                Dictionary<string, int> parameters = new Dictionary<string, int>();
                foreach (StackPanel stackPanel in AlgorithmParamsPanel.Children)
                {
                    string? name = stackPanel.Children.OfType<Label>().First().Content.ToString();
                    name = name == null ? string.Empty : name; // Ensure name is not null
                    int value = int.Parse(stackPanel.Children.OfType<TextBox>().First().Text);
                    parameters[name] = value;
                }
                manager.setAlgorithmParams(AlgorithmChoice.SelectedIndex, parameters);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error parsing integer", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }


        // /////////////// Instance edition/saving/loading section ///////////////

        private void TasksDataGrid_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            try
            {
                var val = Math.Max(1, int.Parse(((TextBox)e.EditingElement).Text));
                ((TextBox)e.EditingElement).Text = val.ToString();
                taskTimes[e.Row.GetIndex(), e.Column.DisplayIndex - 1] = val;
                createGanttVisualisationStages();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error parsing integer", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void MachinesDataGrid_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            var val = Math.Max(1, int.Parse(((TextBox)e.EditingElement).Text));
            ((TextBox)e.EditingElement).Text = val.ToString();
            machines[e.Column.DisplayIndex] = val;
            createGanttVisualisationStages();
        }

        /// <summary>
        /// Calls the Windows save window and calls the SaveBinaryFile function
        /// </summary>
        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            var saveFileDialog = new Microsoft.Win32.SaveFileDialog
            {
                Filter = "Data Files (*.dat)|*.dat|All Files (*.*)|*.*",
                Title = "Save Binary File"
            };

            if (saveFileDialog.ShowDialog() == true)
            {
                try
                {
                    SaveBinaryFile(saveFileDialog.FileName);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "File save error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        /// <summary>
        /// Calls the Windows open window and calls the LoadBinaryFile function.<br/>
        /// Fills the datagrids and fills refreshes the gant visualisation.<br/>
        /// Fills generator values with edge values from the file.
        /// </summary>
        private void LoadButton_Click(object sender, RoutedEventArgs e)
        {
            var openFileDialog = new Microsoft.Win32.OpenFileDialog
            {
                Filter = "Data Files (*.dat)|*.dat|All Files (*.*)|*.*",
                Title = "Open Binary File"
            };

            if (openFileDialog.ShowDialog() == true)
            {
                try
                {
                    LoadBinaryFile(openFileDialog.FileName);
                    fillInstanceDataGrids();
                    createGanttVisualisationStages();
                    numOfStages.Text = machines.Length.ToString();
                    numOfTasks.Text = taskTimes.GetLength(0).ToString();
                    maxNumOfMachinesInStage.Text = machines.Max().ToString();
                    maxTaskTime.Text = taskTimes.Cast<int>().Max().ToString();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "File load error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        /// <summary>
        /// Saves the data from machines and taskTimes arrays to a binary file specified by the filePath argument
        /// </summary>
        private void SaveBinaryFile(string filePath)
        {
            using (BinaryWriter writer = new BinaryWriter(File.Open(filePath, FileMode.Create)))
            {
                //writer.Write("MM");
                // Write the length of the machines array
                writer.Write(machines.Length);

                // Write the machines array
                foreach (int machine in machines)
                {
                    writer.Write(machine);
                }

                // Write the dimensions of the taskTimes array
                writer.Write(taskTimes.GetLength(0)); // Number of rows
                writer.Write(taskTimes.GetLength(1)); // Number of columns

                // Write the taskTimes array
                for (int i = 0; i < taskTimes.GetLength(0); i++)
                {
                    for (int j = 0; j < taskTimes.GetLength(1); j++)
                    {
                        writer.Write(taskTimes[i, j]);
                    }
                }
            }
        }

        /// <summary>
        /// Loads the data to machines and taskTimes arrays from a binary file specified by the filePath argument
        /// </summary>
        private void LoadBinaryFile(string filePath)
        {
            using (BinaryReader reader = new BinaryReader(File.Open(filePath, FileMode.Open)))
            {
                // Read the length of the machines array
                //string s = reader.ReadString();
                int machinesLength = reader.ReadInt32();
                int[] machinesFromFile = new int[machinesLength];
                for (int i = 0; i < machinesLength; i++)
                {
                    machinesFromFile[i] = reader.ReadInt32();
                }

                // Read the dimensions of the taskTimes array
                int rows = reader.ReadInt32();
                int cols = reader.ReadInt32();
                int[,] taskTimesFromFile = new int[rows, cols];

                // Read the taskTimes array
                for (int i = 0; i < rows; i++)
                {
                    for (int j = 0; j < cols; j++)
                    {
                        taskTimesFromFile[i, j] = reader.ReadInt32();
                    }
                }
                // Validate data
                machines = machinesFromFile;
                taskTimes = taskTimesFromFile;
            }
        }


        // /////////////// Run section ///////////////

        /// <summary>
        /// Called on run button click<br/>
        /// <list type="number">
        /// <item>Starts the timer</item>
        /// <item>Disables buttons</item>
        /// <item>Starts the algorithm</item>
        /// <item>Stops the timer</item>
        /// <item>Displays the effects of the program</item>
        /// <item>Enables the buttons</item>
        /// </list>
        /// </summary>
        private void Run_Button_Click(object sender, RoutedEventArgs e)
        {
            if (runThread != null && runThread.IsAlive) return;
            int algId = AlgorithmChoice.SelectedIndex;

            runTimerTriggered = false;
            runTimer.Start();
            runStartTime = DateTime.Now;

            runThread = new Thread(new ThreadStart(() =>
            {
                Dispatcher.Invoke(() =>
                {
                    RunButton.IsEnabled = false;
                    StopButton.IsEnabled = true;
                    GenerateButton.IsEnabled = false;
                    SetParamsButton.IsEnabled = false;
                    MachinesDataGrid.IsEnabled = false;
                    TasksDataGrid.IsEnabled = false;

                });
                Tuple<int, int>[,]? result = null;
                int[] solvedPermutation = new int[taskTimes.GetLength(0)];
                try
                {
                    solvedPermutation = new int[5];
                    (result, solvedPermutation) = manager.run(algId, machines, taskTimes);
                } catch (Exception ex)
                {
                    if (ex.Message != "canceled")
                    {
                        MessageBox.Show(ex.Message, "Run Error", MessageBoxButton.OK, MessageBoxImage.Error);
                    }
                }
                Dispatcher.Invoke(() =>
                {
                    runTimer.Stop();
                    if (!runTimerTriggered) TimerDisplay.Content = "< 50ms";
                    visualizeSchedule(result);
                    SolvedPermutationLabel.Content = string.Join(", ", solvedPermutation);
                    RunButton.IsEnabled = true;
                    StopButton.IsEnabled = false;
                    GenerateButton.IsEnabled = true;
                    SetParamsButton.IsEnabled = true;
                    MachinesDataGrid.IsEnabled = true;
                    TasksDataGrid.IsEnabled = true;
                });
            }));
            runThread.Start();
        }

        /// <summary>
        /// Called on every tick of runTimer<br/>
        /// Writes the value into the GUI timer
        /// </summary>
        private void runTimer_Tick(object sender, EventArgs e)
        {
            runTimerTriggered = true;
            TimerDisplay.Content = Convert.ToString(DateTime.Now - runStartTime);
        }

        /// <summary>
        /// Called on onclick of StopButton<br/>
        /// Sends the cancel signal to the backend
        /// </summary>
        private void StopButton_Click(object sender, RoutedEventArgs e)
        {
            manager.cancelAlgorithm();
        }

        /// <summary>
        /// Called when window is closed<br/>
        /// Sends the cancel signal to the backend
        /// </summary>
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            manager.cancelAlgorithm();
        }


        // /////////////// Answer display section ///////////////

        /// <summary>
        /// Populates the Gantt chart with the tasks in the way specified by the schedule in result variable
        /// </summary>
        /// <param name="result">Schedule: an array of tasks each having an array of operations each having a tuple of nachine id for the stage and time of execution start</param>
        private void visualizeSchedule(Tuple<int, int>[,]? result)
        {
            if (result == null)
            {
                AnswerText.Text = "no results";
                return;
            }
            AnswerText.Text = answerTuple2string(result);

            for (int stageIndex = 0; stageIndex < result.GetLength(1); stageIndex++)
            {
                for (int i = 0; i < machines[stageIndex]; i++)
                {
                    object node = FindName("machineRow_stage" + stageIndex + "_machine" + i);
                    (node as Canvas).Children.Clear();
                }
            }

            List<SolidColorBrush> colorTable = GenerateEvenlySpacedColorTable(taskTimes.GetLength(0));

            int visualisationScale = 7;
            int cmax = 0;
            for (int taskIndex = 0; taskIndex < result.GetLength(0); taskIndex++)
            {
                for (int stageIndex = 0; stageIndex < result.GetLength(1); stageIndex++)
                {
                    object node = FindName("machineRow_stage" + stageIndex + "_machine" + result[taskIndex, stageIndex].Item1);
                    if (node is Canvas)
                    {
                        Canvas machineRow = node as Canvas;
                        int taskTime = taskTimes[taskIndex, stageIndex];
                        int leftPosition = result[taskIndex, stageIndex].Item2;
                        cmax = leftPosition + taskTime < cmax ? cmax : leftPosition + taskTime;
                        Canvas task = new Canvas
                        {
                            Name = "machine" + result[taskIndex, stageIndex].Item1 + "_stage" + stageIndex,
                            Height = machineRow.Height,
                            Width = taskTime * visualisationScale,
                            Background = colorTable[taskIndex],
                        };
                        Canvas.SetLeft(task, leftPosition * visualisationScale);
                        task.Children.Add(new TextBlock { Text = taskIndex.ToString() });
                        machineRow.Children.Add(task);
                    }
                }
            }
            VisualisationStackPanel.MinWidth = cmax * visualisationScale > VisualisationStackPanel.ViewportWidth ? cmax * visualisationScale + visualisationScale : VisualisationStackPanel.ViewportWidth + visualisationScale;
            cMaxDisplay.Content = cmax;
        }

        /// <summary>
        /// Returns the string for crude visualisation from the schedule array
        /// </summary>
        public static string answerTuple2string(Tuple<int, int>[,]? tuples)
        {
            if (tuples == null) return "no results";
            StringBuilder sb = new StringBuilder();

            // Get the dimensions of the 2D array
            int rows = tuples.GetLength(0);
            int cols = tuples.GetLength(1);

            // Loop through the array to format each tuple
            for (int i = 0; i < rows; i++)
            {
                sb.Append("T" + i + " [");
                for (int j = 0; j < cols; j++)
                {
                    var tuple = tuples[i, j];
                    sb.Append($"({tuple.Item1}, {tuple.Item2})");

                    // Add a space between tuples except for the last one in the row
                    if (j < cols - 1)
                    {
                        sb.Append("  ");
                    }
                }
                sb.Append("]");
                // Add a new line after each row
                sb.AppendLine();
            }

            return sb.ToString();
        }

        // Helper function to generate the colortable used by the gantt visualisation
        List<SolidColorBrush> GenerateEvenlySpacedColorTable(int colorCount)
        {
            List<SolidColorBrush> colorTable = new List<SolidColorBrush>();
            double saturation = 0.7; // Fixed saturation for vivid colors
            double lightness = 0.5;  // Fixed lightness for balanced brightness

            for (int i = 0; i < colorCount; i++)
            {
                double hue = (i * 360.0 / colorCount) % 360; // Evenly spaced hues
                colorTable.Add(new SolidColorBrush(HslToRgb(hue, saturation, lightness)));
            }

            return colorTable;
        }

        // Helper function to convert HSL to RGB
        Color HslToRgb(double hue, double saturation, double lightness)
        {
            double c = (1 - Math.Abs(2 * lightness - 1)) * saturation;
            double x = c * (1 - Math.Abs((hue / 60.0) % 2 - 1));
            double m = lightness - c / 2;
            double r = 0, g = 0, b = 0;

            if (0 <= hue && hue < 60) { r = c; g = x; b = 0; }
            else if (60 <= hue && hue < 120) { r = x; g = c; b = 0; }
            else if (120 <= hue && hue < 180) { r = 0; g = c; b = x; }
            else if (180 <= hue && hue < 240) { r = 0; g = x; b = c; }
            else if (240 <= hue && hue < 300) { r = x; g = 0; b = c; }
            else if (300 <= hue && hue < 360) { r = c; g = 0; b = x; }

            byte red = (byte)((r + m) * 255);
            byte green = (byte)((g + m) * 255);
            byte blue = (byte)((b + m) * 255);

            return Color.FromRgb(red, green, blue);
        }

        // Helper function for right Gantt visualisation scroll behaviour
        private void VisualisationScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            if (sender == VisualisationStackPanelScrollViewer)
                VisualisationStackPanelStageScrollViewer.ScrollToVerticalOffset(e.VerticalOffset);
            else
                VisualisationStackPanelScrollViewer.ScrollToVerticalOffset(e.VerticalOffset);
        }

        // Helper function for right Gantt visualisation scroll behaviour
        private void VisualisationStackPanel_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (VisualisationStackPanelScrollViewer.ComputedHorizontalScrollBarVisibility == Visibility.Visible)
            {
                VisualisationStackPanelStageScrollViewer.HorizontalScrollBarVisibility = ScrollBarVisibility.Visible;
            }
            else
            {
                VisualisationStackPanelStageScrollViewer.HorizontalScrollBarVisibility = ScrollBarVisibility.Hidden;
            }
        }

        // /////////////// Validation section ///////////////
        private void PositiveNumberValidationTextBoxChange(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("^[0-9]+$");
            e.Handled = !regex.IsMatch(e.Text);
        }

        private void NumberValidationTextBoxChange(object sender, TextCompositionEventArgs e)
        {
            // Allow only a leading minus sign and digits
            Regex regex = new Regex("^-?[0-9]*$");
            e.Handled = !regex.IsMatch(GetProposedText(sender as TextBox, e.Text));
        }

        // Helper method to get the proposed text after the input
        private string GetProposedText(TextBox textBox, string inputText)
        {
            if (textBox == null) return inputText;

            // Determine the text as if the new input was added
            var textBefore = textBox.Text.Substring(0, textBox.SelectionStart);
            var textAfter = textBox.Text.Substring(textBox.SelectionStart + textBox.SelectionLength);
            return textBefore + inputText + textAfter;
        }

        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (sender is TextBox textBox)
            {
                try
                {
                    string text = textBox.Text;
                    if (string.IsNullOrWhiteSpace(text) || text == "-")
                    {
                        text = "0";
                    }
                    int value = int.Parse(text);
                    textBox.Text = value.ToString();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Not a number", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

    }
}