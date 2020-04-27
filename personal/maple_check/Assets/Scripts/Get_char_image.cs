using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

public class Get_char_image : MonoBehaviour
{
    public GameObject target;
    public Image out_img;
    bool state_flag, is_loaded = false;

    // Use this for initialization
    void Start()
    {

    }

    IEnumerator set_img(string img_url)
    {
        WWW char_img = new WWW(img_url);
        //out_img = char_img.

        do
        {
            yield return null;
        } while (!char_img.isDone);

        //gameObject.GetComponent<SpriteRenderer>(). = char_img.texture as SpriteRenderer;


        //보류
        //Texture2D to_fill = new Texture2D(180, 180);
        //char_img.LoadImageIntoTexture(to_fill);


        //out_img.sprite = to_fill as Sprite;

        //된것
        //char_img.LoadImageIntoTexture(out_img.sprite.texture);

        //char_img.texture.LoadImage();
        Debug.Log("img loaded");

        char_img.Dispose();
        char_img = null;
    }

    // Update is called once per frame
    void Update()
    {
        if (!state_flag)
        {
            if (GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_parse_state() == 3)
            {
                state_flag = true;
                is_loaded = true;
                Debug.Log("state trasintted to 3");
            }
            else
                return;
        }
        if (is_loaded)
        {
            string img_url = GameObject.Find("PlayerPawn").GetComponent<PlayerPawnScript>().get_char_image();

            StartCoroutine(set_img(img_url));
            is_loaded = false;
        }
    }
}
