using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class Tile_loader : MonoBehaviour
{
    /*
 Tile_map : 
 * 
    0 : none
    1 : hole
    2 : Blue
    3 : Red
    4 : Green
    5 : Yellow
    6 : Purple
    7 : Sausage
    8 : Candy ball
    9 : Rainbow
 Backtile : 
 * 0: A
 * 1: B
 * 2: hole
 */
    public string Tile_level = "";
    private int width = 0;
    private int height = 0;
    private string base_path;

    private List<List<int>> Tile_map = new List<List<int>>();

    // Use this for initialization
    void Start()
    {


    }

    public void Init()
    {


        base_path = Application.dataPath + "/Tiles/";
        Debug.Log(base_path + Tile_level + ".csv");

        StreamReader reader = new StreamReader(base_path + Tile_level + ".csv");

        do
        {
            string a_line = reader.ReadLine();
            List<int> a_line_int = new List<int>();
            int now_length = 0;

            if (a_line == null)//비어있으면 중지
                break;

            for (int i = 0; i < a_line.Length; i++)//,를 제외한 숫자를 차례대로 입력
            {
                if (a_line[i] != ',')
                {
                    a_line_int.Add(a_line[i] - '0');
                    now_length++;
                }
            }
            if (now_length > width)//최대 길이를 width로 저장
                width = now_length;
            //Debug.Log(a_line_int[2]);
            Tile_map.Add(a_line_int);
            //Tile_map.

            //Tile_map.Add(a_line);
        } while (true);

        height = Tile_map.Count;

        //Debug.Log("width:" + width);
        //Debug.Log("height:" + height);


        //int ind = 0;
        //while (ind < Tile_map.Count)
        //{
        //    Debug.Log(Tile_map[ind++]);
        //}

        Print_all();
    }

    public void Print_all()
    {
        for(int j = 0; j < height; j++)
        {
            string print_string = "";

            for (int i = 0; i < width; i++)
            {
                print_string = print_string + Tile_map[j][i].ToString();
            }
            //Debug.Log("line" + (j+1).ToString() + " :"+ print_string);
        }
    }

    // Update is called once per frame
    void Update()
    {

    }

    public List<List<int>> Get_Tile_map()
    {
        return Tile_map;
    }

    public int Get_width()
    {
        return width;
    }
    public int Get_height()
    {
        return height;
    }
}

