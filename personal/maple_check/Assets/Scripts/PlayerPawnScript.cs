using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerPawnScript : MonoBehaviour {
    public string character_name;
    public string url;
    public string data_path;
    public string parse_data_name;
    // Use this for initialization
    void Start () {
        url = "https://maplestory.nexon.com/Ranking/World/Total?c=";
        data_path = Application.dataPath + "/datafiles/";
        parse_data_name = "1st parse.txt";
    }
	
	// Update is called once per frame
	void Update () {
		
	}
}
