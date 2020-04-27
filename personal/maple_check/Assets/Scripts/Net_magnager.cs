using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;


public class Net_magnager : MonoBehaviour {

    string character_name, url;
    public string page_data;
    // Use this for initialization
    void Start () {

        character_name = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().character_name;
        url = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().url;
        StartCoroutine(Get_html());
    }

    IEnumerator Get_html()
    {
        WWW html_data = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_html();
        do
        {
            yield return null;
        }
        while (!html_data.isDone);

        if (html_data.error != null)
        {
            Debug.Log("web.error" + html_data.error);
            Debug.Log("\n nowurl :" + url + character_name);
            yield break;
        }
        else
        {
            Debug.Log("net_connected");
            page_data = html_data.text;
            GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().set_parse_state(1);

            //yield break;
        }
    }

    // Update is called once per frame
    void Update () {
		
	}
}
