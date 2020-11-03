using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoyReaction : MonoBehaviour
{
    private Animator animator;
    public GameObject stone1;
    public GameObject stone2;
    bool one = true;
    private bool reaction = false;
    StoneScript stone_script1;
    StoneScript stone_script2;


    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();
        stone1 = GetComponent<GameObject>();
        stone2 = GetComponent<GameObject>();
        stone_script1 = GameObject.Find("StoneOne").GetComponent<StoneScript>();
        stone_script2 = GameObject.Find("StoneTwo").GetComponent<StoneScript>();
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Space) && !reaction)
        {
            animator.SetBool("reaction", true);
            reaction = true;
        }
        else if (Input.GetKeyDown(KeyCode.Space) && reaction)
        {
            animator.SetBool("reaction", false);
        }
        if(Input.GetKeyDown(KeyCode.B))
        {
            animator.SetBool("jumping", true);
        }
    }

    public void ThrowStone()
    {
        if (one)
        {
            stone_script1.Release();
            one = false;
        }
        else
            stone_script2.Release();
        
    }
}
